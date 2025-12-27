#include "include/display.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "include/tasks_common.h"
#include "stdint.h"

#define MOSI              23
#define CLK               18 // Clock
#define RCK               4  // Latch
#define PIXEL_COUNT       8
#define CONTROL_BUFF_SIZE 2
#define FPS_12_DELAY      996

static const char *TAG = "DSP";

static QueueHandle_t display_control_queue_handle;

// XXX frame by frame, or text pan
static uint8_t mode = 0;

// XXX pointer to a 8 byte frame, 1 byte per column
static frame_t *curr_frame;

static spi_device_handle_t spi_handle;

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

static void display_control_task(void *pvParameter) {
    ESP_LOGI(TAG, "Display Control listening");

    uint8_t message;

    for (;;) {
        if (xQueueReceive(display_control_queue_handle, &message, portMAX_DELAY)) {
            switch (message) {
            case 0:
                break;
            }
        }
    }
}

static void display_run_task(void *pvParameter) {
    for (;;) {
        if (mode == 0) {
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

void display_run_task_start() {
    xTaskCreatePinnedToCore(&display_run_task,
                            "SPI_DISPLAY_RUN",
                            DISPLAY_RUN_TASK_STACK_SIZE,
                            NULL,
                            DISPLAY_RUN_READ_TASK_PRIORITY,
                            NULL,
                            DISPLAY_RUN_READ_TASK_CORE_ID);
}

void display_cmd_send(uint8_t message) {
    xQueueSend(display_control_queue_handle, &message, portMAX_DELAY);
}
