#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/sim800l.h"
#include "include/voltage.h"
#include <stdio.h>

static const char *TAG = "SIM800L";

#define BUTTON_GPIO GPIO_NUM_25

/**
 * Calls a phone number on EXT.
 *
 * AT - Basic Check
 * AT+CSQ - Signal Check
 * AT+CPIN? - Pin/Puk check
 */
void app_main(void) {
    // TODO add a battery level check widh LEDS's and a resistor divider
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_pullup_en(BUTTON_GPIO);

    // XXX multiple buttons would require 'esp_sleep_enable_ext1_wakeup', level high only:
    // esp_sleep_enable_ext1_wakeup((1ULL<<32) | (1ULL<<33), ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_ext0_wakeup(BUTTON_GPIO, 0);

    esp_err_t err = 0;
    err = sim800l_init();

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "SIM800L Init fail.");
        return;
    } else {
        ESP_LOGI(TAG, "SIM800L OK.");
    }

    err = voltage_init();

    if (err != ESP_OK) {
        ESP_LOGI(TAG, "ADC Init fail.");
        return;
    }

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        ESP_LOGI(TAG, "Awoken");

        sim800l_with_retry("AT+CREG?", "0,1");

        sim800l_task_start();

        sim800l_send_cmd("ATD+37126312241;");
        vTaskDelay(pdMS_TO_TICKS(5000));

        esp_deep_sleep_start();
        ESP_LOGI(TAG, "Going to sleep...");
    } else {

        while (1) {
            ESP_LOGI(TAG, "V: %f", read_battery_voltage());
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        esp_deep_sleep_start();
    }
}