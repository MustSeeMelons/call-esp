#include "esp_err.h"

typedef struct {
    uint8_t col;
    uint8_t row;
} frame_t;

esp_err_t display_init();

void display_control_task_start();

void display_run_task_start();

void display_cmd_send(uint8_t message);