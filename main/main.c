#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/display.h"
#include "include/sim800l.h"
#include "include/voltage.h"
#include <stdio.h>

static const char *TAG = "MAIN";

#define BUTTON_GPIO GPIO_NUM_12
static QueueHandle_t gpio_evt_queue;

static void IRAM_ATTR gpio_isr_handler(void *arg);
static volatile bool isBeat = true;

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

static void IRAM_ATTR gpio_isr_handler(void *arg) {
    uint32_t pin = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &pin, NULL);
}

/**
 * this should be split - init stays here, phone call to BLE handlers
 * AT - Basic Check
 * AT+CSQ - Signal Check
 * AT+CPIN? - Pin/Puk check
 */
static void do_phone() {
    sim800l_with_retry("AT+CREG?", "0,1");

    sim800l_task_start();

    sim800l_send_cmd("ATD+37126312241;");
}

void app_main(void) {
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    gpio_interrupt_init();

    esp_err_t err = 0;
    // err = sim800l_init();

    // if (err != ESP_OK) {
    //     ESP_LOGI(TAG, "SIM800L Init fail.");
    //     return;
    // } else {
    //     ESP_LOGI(TAG, "SIM800L OK.");
    // }

    err = display_init();

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Display Init fail.");
        return;
    } else {
        ESP_LOGI(TAG, "Display OK.");
        display_control_task_start();
        display_run_task_start();
        display_ticker_task_start();
    }

    display_cmd_send(3);

    while (1) {
        uint32_t pin;
        TickType_t last_time = 0;

        if (xQueueReceive(gpio_evt_queue, &pin, portMAX_DELAY)) {
            TickType_t now = xTaskGetTickCount();

            // Debounce check
            if ((now - last_time) > pdMS_TO_TICKS(50)) {
                vTaskDelay(pdMS_TO_TICKS(10));

                int level = gpio_get_level(BUTTON_GPIO);
                if (level == 0) {
                    if (isBeat) {
                        display_cmd_send(2);
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