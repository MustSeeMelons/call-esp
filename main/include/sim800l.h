#ifndef SIM800L_H_
#define SIM800L_H_

#include "esp_err.h"
#include "include/sim800l.h"

#define UART_NUM UART_NUM_1
#define BUF_SIZE 1024

esp_err_t sim800l_init();

void sim800l_task_start();

void sim800l_send_cmd(const char *cmd);

void sim800l_with_retry(const char *cmd, const char *response);

#endif