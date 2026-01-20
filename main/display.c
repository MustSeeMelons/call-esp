#include "include/display.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "include/letters.h"
#include "include/tasks_common.h"
#include "include/util.h"
#include "stdint.h"

#define MOSI              22
#define CLK               18 // Clock
#define RCK               4  // Latch
#define PIXEL_COUNT       8
#define CONTROL_BUFF_SIZE 2
#define FPS_6_DELAY       84

static const char *TAG = "DSP";

static QueueHandle_t display_control_queue_handle;

static spi_device_handle_t spi_handle;

extern dsp_item_t t_welcome;
extern dsp_item_t t_error;
extern dsp_item_t t_gate;
extern dsp_item_t t_cool;

extern dsp_anim_data breathe;
extern dsp_anim_data wave;
extern dsp_anim_data line;
extern dsp_anim_data sync_line;
extern dsp_anim_data load;
extern dsp_anim_data standby;

scroll_text_t scroll = {.letters = NULL, .count = 0, .scroll_pos = 0};
anim_t anim = {.frame = NULL, .count = 0, .active = 0};

static dsp_mode_t mode = DSP_IDLE;

static esp_err_t init_latch() {
    esp_err_t err;

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << RCK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    err = gpio_config(&io_conf);

    return err;
}

esp_err_t display_init() {
    display_control_queue_handle = xQueueCreate(CONTROL_BUFF_SIZE, sizeof(uint8_t));
    esp_err_t err;

    spi_bus_config_t buscfg = {
        .mosi_io_num = MOSI,
        .miso_io_num = -1,
        .sclk_io_num = CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 500 * 1000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1,
        .flags = 0,
    };

    err = init_latch();

    if (err != ESP_OK) {
        return err;
    }

    return spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
}

static void display_update_scroll(dsp_item_t *text) {
    scroll.letters = text->data.scroll.letters;
    scroll.count = text->data.scroll.count;

    int total_width = text->data.scroll.count - 1;

    for (size_t i = 0; i < text->data.scroll.count; i++) {
        total_width += letter_get_len(text->data.scroll.letters[i]);
    }

    scroll.scroll_pos = total_width + PIXEL_COUNT;
    mode = text->dsp_mode;
}

static void display_update_anim(dsp_anim_data *anim_data) {
    anim.frame = anim_data->frame;
    anim.count = anim_data->count;
    anim.active = 0;
    anim.frame_delay = 0;
    mode = DSP_FRAME_ANIM;
}

// Set mode & other variables
static void display_control_task(void *pvParameter) {
    ESP_LOGI(TAG, "Display Control listening");

    uint8_t message;

    for (;;) {
        if (xQueueReceive(display_control_queue_handle, &message, portMAX_DELAY)) {

            switch (message) {
            case e_welcome:
                display_update_scroll(&t_welcome);
                break;
            case e_breathe:
                display_update_anim(&breathe);
                break;
            case e_wave:
                display_update_anim(&wave);
                break;
            case e_line:
                display_update_anim(&line);
                anim.frame_delay = 42;
                break;
            case e_sync_line:
                display_update_anim(&sync_line);
                anim.frame_delay = 42;
                break;
            case e_load:
                display_update_anim(&load);
                anim.frame_delay = 60;
                break;
            case e_error:
                display_update_scroll(&t_error);
                break;
            case e_gate:
                display_update_scroll(&t_gate);
                break;
            case e_cool:
                display_update_scroll(&t_cool);
                break;
            case e_standby:
                display_update_anim(&standby);
                anim.frame_delay = 100;
                break;
            }
        }
    }
}

static void render_scrolling_text(scroll_text_t *text) {
    uint8_t frame[8] = {0};

    int pixel_pos = -text->scroll_pos;

    for (int8_t i = text->count - 1; i >= 0; i--) {
        letter_t *letter = text->letters[i];
        uint8_t offset = letter_get_offset(letter);
        uint8_t len = letter_get_len(letter);

        for (uint8_t col = 0; col < len; col++) {
            int display_col = pixel_pos + col;

            if (display_col >= 0 && display_col < PIXEL_COUNT) {
                frame[display_col] = letter->data[offset + col];
            }
        }

        pixel_pos += len + 1;
    }

    static uint8_t tx[2];

    for (size_t col_idx = 0; col_idx < PIXEL_COUNT; col_idx++) {
        uint8_t row = 1 << col_idx;
        uint8_t col = reverse_u8(frame[col_idx]);

        tx[0] = col;
        tx[1] = row;

        spi_transaction_t t = {
            .length = 2 * 8,
            .tx_buffer = tx,
        };

        spi_device_transmit(spi_handle, &t);

        gpio_set_level(RCK, 1);
        gpio_set_level(RCK, 0);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void display_run_task(void *pvParameter) {
    for (;;) {
        switch (mode) {
        case DSP_TEST:
            for (size_t row_idx = 0; row_idx < PIXEL_COUNT; row_idx++) {
                for (size_t col_idx = 0; col_idx < PIXEL_COUNT; col_idx++) {

                    uint8_t row = 1 << row_idx;
                    uint8_t col = 1 << (PIXEL_COUNT - col_idx - 1);

                    uint8_t tx[2] = {col, row};

                    spi_transaction_t t = {
                        .length = 2 * 8,
                        .tx_buffer = tx,
                    };

                    spi_device_transmit(spi_handle, &t);

                    // Update to display
                    gpio_set_level(RCK, 1);
                    gpio_set_level(RCK, 0);

                    vTaskDelay(pdMS_TO_TICKS(25));
                }
            }
            break;
        case DSP_FRAME_ANIM:
            for (size_t col_idx = 0; col_idx < PIXEL_COUNT; col_idx++) {
                uint8_t row = 1 << col_idx;

                uint8_t col = reverse_u8(anim.frame[anim.active][col_idx]);

                uint8_t tx[2] = {col, row};

                spi_transaction_t t = {
                    .length = 2 * 8,
                    .tx_buffer = tx,
                };

                spi_device_transmit(spi_handle, &t);

                // Update to display
                gpio_set_level(RCK, 1);
                gpio_set_level(RCK, 0);

                vTaskDelay(pdMS_TO_TICKS(1));
            }
            break;
        case DSP_TEXT_SCROLL:
            render_scrolling_text(&scroll);
            break;
        default:
            // Chill if we have nothing to do
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
        }
    }
}

void display_control_task_start() {
    xTaskCreatePinnedToCore(&display_control_task,
                            "SPI_DISPLAY_CONTROL",
                            DISPLAY_CONTROL_TASK_STACK_SIZE,
                            NULL,
                            DISPLAY_CONTROL_TASK_PRIORITY,
                            NULL,
                            DISPLAY_CONTROL_TASK_CORE_ID);
}

void update_scroll(scroll_text_t *text) {
    text->scroll_pos--;

    // Calculate total width of text
    int total_width = text->count - 1;
    for (size_t i = 0; i < text->count; i++) {
        total_width += letter_get_len(text->letters[i]);
    }

    // Reset when text has scrolled off screen
    if (text->scroll_pos <= -8) {
        text->scroll_pos = total_width + PIXEL_COUNT;
    }
}

void update_anim(anim_t *anim) {
    anim->active++;

    if (anim->active >= anim->count) {
        anim->active = 0;
    }
}

static void display_ticker_task(void *pvParameter) {
    for (;;) {
        switch (mode) {
        case DSP_TEXT_SCROLL:
            vTaskDelay(pdMS_TO_TICKS(75));
            update_scroll(&scroll);
            break;
        case DSP_FRAME_ANIM:
            uint16_t delay = anim.frame_delay != 0 ? anim.frame_delay : FPS_6_DELAY;
            vTaskDelay(pdMS_TO_TICKS(delay));
            update_anim(&anim);
            break;
        default:
            vTaskDelay(pdMS_TO_TICKS(100));
            break;
        }
    }
}

void display_run_task_start() {
    xTaskCreatePinnedToCore(&display_run_task,
                            "SPI_DISPLAY_RUN",
                            DISPLAY_RUN_TASK_STACK_SIZE,
                            NULL,
                            DISPLAY_RUN_TASK_PRIORITY,
                            NULL,
                            DISPLAY_RUN_TASK_CORE_ID);
}

void display_ticker_task_start() {
    xTaskCreatePinnedToCore(&display_ticker_task,
                            "SPI_DISPLAY_TICKER",
                            DISPLAY_TICKER_TASK_STACK_SIZE,
                            NULL,
                            DISPLAY_TICKER_READ_TASK_PRIORITY,
                            NULL,
                            DISPLAY_TICKER_READ_TASK_CORE_ID);
}

void display_cmd_send(uint8_t message) {
    xQueueSend(display_control_queue_handle, &message, portMAX_DELAY);
}
