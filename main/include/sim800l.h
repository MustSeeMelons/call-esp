#ifndef SIM800L_H_
#define SIM800L_H_

#include "esp_err.h"
#include "include/sim800l.h"
#include "include/util.h"

#define UART_NUM UART_NUM_1
#define BUF_SIZE 128

typedef struct {
    const char *cmd;
    const char *response;
    const uint8_t cmd_len;
    const display_id_t display_result;
} sim800_cmd_t;

typedef enum
{
    SIM800_RESP_OK,
    SIM800_RESP_TIMEOUT,
    SIM800_RESP_INCOMPLETE
} sim800_resp_type_t;

esp_err_t sim800l_init();

void sim800l_task_start();

void sim800l_send_cmd(sim800_cmd_t cmd);

void sim800l_call_first();

void sim800l_call_other();

sim800_resp_type_t sim800l_with_retry(sim800_cmd_t cmd);

#endif