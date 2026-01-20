#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/blu.h"
#include "include/display.h"
#include "include/sim800l.h"
#include "include/util.h"
#include "include/voltage.h"
#include "nvs_flash.h"
#include <stdio.h>

// Comment out for quick display testing
#define BLU_ENABLE
#define SIM_ENABLE

static const char *TAG = "Main";

#define BUTTON_GPIO GPIO_NUM_12
static QueueHandle_t gpio_evt_queue;

static sim800_cmd_t cmd_reg = {.cmd = "AT+CREG?", .cmd_len = 8, .response = "0,1", .display_result = e_standby};

static volatile bool isBeat = true; // Terrible name, but it will do

static void IRAM_ATTR gpio_isr_handler(void *arg) {
    uint32_t pin = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &pin, NULL);
}

void gpio_interrupt_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    gpio_config(&io_conf);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, (void *)BUTTON_GPIO);
}

void app_main(void) {
    esp_err_t err = 0;

    // Bluetooth requires NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", err);
        return;
    } else {
        ESP_LOGI(TAG, "NVS OK.");
    }

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    gpio_interrupt_init();

    err = display_init();

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Display init failed.");
        return;
    } else {
        ESP_LOGI(TAG, "Display OK.");
        display_control_task_start();
        display_run_task_start();
        display_ticker_task_start();
    }

    display_cmd_send(e_load);

#ifdef BLU_ENABLE
    err = blu_init();

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "BLE init failed.");
        return;
    }
#endif

#ifdef SIM_ENABLE
    err = sim800l_init();

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "SIM800L Init fail.");
        return;
    } else {
        ESP_LOGI(TAG, "SIM800L OK.");
    }
#endif

#ifdef SIM_ENABLE
    sim800l_task_start();

    sim800l_send_cmd(cmd_reg);
#endif
    // Button handling code
    while (1) {
        uint32_t pin;
        TickType_t last_time = 0;

        if (xQueueReceive(gpio_evt_queue, &pin, portMAX_DELAY)) {
            TickType_t now = xTaskGetTickCount();

            // Debounce check
            if ((now - last_time) > pdMS_TO_TICKS(50)) {
                vTaskDelay(pdMS_TO_TICKS(20));

                int level = gpio_get_level(BUTTON_GPIO);
                if (level == 0) {
                    if (isBeat) {
                        display_cmd_send(6);
                        isBeat = false;
                    } else {
                        display_cmd_send(3);
                        isBeat = true;
                    }

                    last_time = now;
                }
            }
        }
    }
}