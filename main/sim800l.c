#include "include/sim800l.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "include/defs.h"
#include "include/display.h"
#include "include/ring_buffer.h"
#include "include/tasks_common.h"
#include "string.h"

#define SIM_TX    GPIO_NUM_26
#define SIM_RX    GPIO_NUM_27
#define SIM_PWKEY GPIO_NUM_4
#define SIM_POWER GPIO_NUM_23
#define SIM_RI    GPIO_NUM_33

// Some commands can take some time
#define SIM_TIMEOUT     10000
#define SIM_RETRY_DELAY 500
#define SIM_RETRY_COUNT 10

static const char *TAG = "SIM800L";
static char rx_buffer[BUF_SIZE];
static ring_buffer_t rx_ring_buffer;
static QueueHandle_t sim800_cmd_queue;

static sim800_cmd_t cmd_call_one = {.cmd = PHONE_FIRST, .cmd_len = 16, .response = "BUSY", .display_result = e_standby};
static sim800_cmd_t cmd_call_other = {
    .cmd = PHONE_SECOND, .cmd_len = 16, .response = "BUSY", .display_result = e_standby};

// Power on sequence
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

// Returns how many bytes to consume for a message if found
static size_t find_message_end(const char *buffer, size_t len, const char *message) {
    if (message == NULL || len == 0) {
        return 0;
    }

    size_t msg_len = strlen(message);
    if (len < msg_len) {
        return 0;
    }

    // XXX OK for small messages only
    for (size_t i = 0; i <= len - msg_len; i++) {
        if (strncmp(&buffer[i], message, msg_len) == 0) {
            // Found message - return length including it
            return i + msg_len;
        }
    }

    return 0;
}

esp_err_t sim800l_init() {
    ring_buffer_init(&rx_ring_buffer, rx_buffer, BUF_SIZE);

    sim800_cmd_queue = xQueueCreate(3, sizeof(sim800_cmd_t));

    esp_err_t err = ESP_OK;

    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    err = uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    if (err != ESP_OK) {
        return err;
    }

    err = uart_param_config(UART_NUM, &uart_config);

    if (err != ESP_OK) {
        return err;
    }

    err = uart_set_pin(UART_NUM, SIM_RX, SIM_TX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    sim800l_power_on();

    return err;
}

static sim800_resp_type_t parse_response(const char *buffer,
                                         size_t buf_data_len,
                                         const char *expected_resp,
                                         size_t *msg_length) {

    if (expected_resp != NULL) {
        size_t exp_len = find_message_end(buffer, buf_data_len, expected_resp);

        if (exp_len > 0) {
            *msg_length = exp_len;

            return SIM800_RESP_OK;

        } else {
            return SIM800_RESP_INCOMPLETE;
        }

        return SIM800_RESP_INCOMPLETE;
    }

    return SIM800_RESP_OK;
}

sim800_resp_type_t sim800l_with_retry(sim800_cmd_t cmd) {
    uint8_t max_retries = SIM_RETRY_COUNT;

    for (uint8_t attempt = 0; attempt < max_retries; attempt++) {
        if (attempt > 0) {
            ESP_LOGW(TAG, "Retry attempt %d/%d for: %s", attempt + 1, max_retries, cmd.cmd);
            vTaskDelay(pdMS_TO_TICKS(SIM_RETRY_DELAY));
        }

        ring_buffer_clear(&rx_ring_buffer);

        ESP_LOGI(TAG, "TX: %s", cmd.cmd);

        uart_write_bytes(UART_NUM, cmd.cmd, strlen(cmd.cmd));
        uart_write_bytes(UART_NUM, "\r\n", 2);

        // Wait for response
        TickType_t start_time = xTaskGetTickCount();
        TickType_t timeout_ticks = pdMS_TO_TICKS(SIM_TIMEOUT);
        char temp_buf[BUF_SIZE];

        while (1) {
            if ((xTaskGetTickCount() - start_time) >= timeout_ticks) {
                ESP_LOGW(TAG, "Timeout waiting for response (attempt %d)", attempt + 1);
                break;
            }

            size_t available = ring_buffer_available(&rx_ring_buffer);
            if (available > 0) {
                size_t peek_len = ring_buffer_peek(&rx_ring_buffer, temp_buf, sizeof(temp_buf) - 1);
                temp_buf[peek_len] = '\0';

                size_t msg_len = 0;
                sim800_resp_type_t resp_type = parse_response(temp_buf, peek_len, cmd.response, &msg_len);

                if (resp_type != SIM800_RESP_INCOMPLETE) {
                    ring_buffer_consume(&rx_ring_buffer, msg_len);
                    ESP_LOGI(TAG, "RX Complete (%d bytes): %.*s", msg_len, msg_len, temp_buf);
                    return resp_type;
                }
            }

            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    // It is what it is
    ESP_LOGE(TAG, "Failed after %d retries: %s", max_retries, cmd.cmd);

    return SIM800_RESP_TIMEOUT;
}

// We read all data into the ring buffer
static void sim800l_read_task(void *pvParameter) {
    uint8_t temp_buf[32];

    while (1) {
        int len = uart_read_bytes(UART_NUM, temp_buf, sizeof(temp_buf), pdMS_TO_TICKS(100));

        if (len > 0) {
            temp_buf[len] = '\0';
            ESP_LOGI(TAG, "RX raw (%d bytes): %s", len, temp_buf);

            ring_buffer_write(&rx_ring_buffer, (char *)temp_buf, len);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void sim800l_send_cmd(sim800_cmd_t cmd) {
    xQueueSend(sim800_cmd_queue, &cmd, portMAX_DELAY);
}

static void sim800l_cmd_task(void *pvParameter) {
    for (;;) {
        sim800_cmd_t cmd;

        if (xQueueReceive(sim800_cmd_queue, &cmd, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Command to send: %s", cmd.cmd);

            if (cmd.response == NULL) {
                // Just send it
                uart_write_bytes(UART_NUM, cmd.cmd, strlen(cmd.cmd));
                uart_write_bytes(UART_NUM, "\r\n", 2);
            } else {
                sim800_resp_type_t status = sim800l_with_retry(cmd);

                if (status != SIM800_RESP_OK) {
                    display_cmd_send(e_error);
                } else if (cmd.display_result != 0) {
                    display_cmd_send(cmd.display_result);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void sim800l_task_start() {
    xTaskCreatePinnedToCore(&sim800l_cmd_task,
                            "SIM800L_CMD",
                            UART_CMD_TASK_STACK_SIZE,
                            NULL,
                            UART_CMD_TASK_PRIORITY,
                            NULL,
                            UART_CMD_TASK_CORE_ID);

    xTaskCreatePinnedToCore(&sim800l_read_task,
                            "SIM800L_READ",
                            UART_READ_TASK_STACK_SIZE,
                            NULL,
                            UART_READ_TASK_PRIORITY,
                            NULL,
                            UART_READ_TASK_CORE_ID);
}

void sim800l_call_first() {
    display_cmd_send(e_gate);
    sim800l_send_cmd(cmd_call_one);
}

void sim800l_call_other() {
    display_cmd_send(e_cool);
    sim800l_send_cmd(cmd_call_other);
}
