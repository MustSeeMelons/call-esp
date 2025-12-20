#ifndef VOLTAGE_H_
#define VOLTAGE_H_

#include "esp_err.h"

esp_err_t voltage_init();

float read_battery_voltage(void);

#endif