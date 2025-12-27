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

void gpio_interrupt_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << GPIO_NUM_12,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE, // or NEGEDGE / ANYEDGE / LOW_LEVEL / HIGH_LEVEL
    };

    gpio_config(&io_conf);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(GPIO_NUM_12, gpio_isr_handler, (void *)GPIO_NUM_12);
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
    }

    display_cmd_send(0);

    while (1) {
        uint32_t pin;
        if (xQueueReceive(gpio_evt_queue, &pin, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Hi!");
        }
    }
}