#ifndef UTIL_H_
#define UTIL_H_

#include "stdint.h"

uint8_t reverse_u8(uint8_t value);

typedef enum display_id
{
    e_nothing = 0,
    e_welcome = 1,
    e_breathe = 3,
    e_wave = 4,
    e_line = 5,
    e_sync_line = 6,
    e_load = 7,
    e_error = 8,
    e_gate = 9,
    e_cool = 10,
    e_standby = 11
} display_id_t;

#endif
