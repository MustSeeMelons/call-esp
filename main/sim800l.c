#include "include/sim800l.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/tasks_common.h"
#include "string.h"

#define SIM_TX    GPIO_NUM_26
#define SIM_RX    GPIO_NUM_27
#define SIM_PWKEY GPIO_NUM_4
#define SIM_POWER GPIO_NUM_23
#define SIM_RI    GPIO_NUM_33

static const char *TAG = "SIM800L";

static void sim800l_power_on(void) {
    gpio_set_direction(SIM_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level(SIM_POWER, 1);

    gpio_set_direction(SIM_PWKEY, GPIO_MODE_OUTPUT);
    gpio_set_level(SIM_PWKEY, 0);

    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(SIM_PWKEY, 1);

    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(SIM_PWKEY, 0);

    // Let the modem do its thing
    vTaskDelay(pdMS_TO_TICKS(5000));
}

esp_err_t sim800l_init() {
    esp_err_t e = -1;

    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    e = uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    if (e != ESP_OK) {
        return e;
    }

    e = uart_param_config(UART_NUM, &uart_config);

    if (e != ESP_OK) {
        return e;
    }

    e = uart_set_pin(UART_NUM, SIM_RX, SIM_TX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    sim800l_power_on();

    return e;
}

void sim800l_send_cmd(const char *cmd) {
    uart_write_bytes(UART_NUM, cmd, strlen(cmd));
    uart_write_bytes(UART_NUM, "\r\n", 2);
}

void sim800l_with_retry(const char *cmd, const char *response) {
    sim800l_send_cmd(cmd);

    uint8_t data[BUF_SIZE];

    while (1) {
        ESP_LOGI(TAG, "Send %s, wait for %s", cmd, response);
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(500));

        if (len > 0) {
            data[len] = '\0'; // Will have some trash otherwise...
            ESP_LOGI(TAG, "Received: %s", (char *)data);

            if (strstr((char *)data, response) != NULL) {
                return;
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(500));
            sim800l_send_cmd(cmd);
        }
    }

    memset(data, 0, BUF_SIZE);
}

static void sim800l_read_task(void *pvParameter) {
    uint8_t data[BUF_SIZE];
    while (1) {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(500));
        if (len > 0) {
            data[len] = '\0';
            ESP_LOGI(TAG, "Received: %s", (char *)data);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void sim800l_task_start() {
    xTaskCreatePinnedToCore(&sim800l_read_task,
                            "SIM800L_READ",
                            UART_READ_TASK_STACK_SIZE,
                            NULL,
                            UART_READ_TASK_PRIORITY,
                            NULL,
                            UART_READ_TASK_CORE_ID);
}