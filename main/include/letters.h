#ifndef LETTERS_H_
#define LETTERS_H_

#include "stdint.h"

typedef struct {
    uint8_t *data;
    uint8_t props;
} letter_t;

uint8_t letter_get_offset(letter_t *l);

uint8_t letter_get_len(letter_t *l);

#endif