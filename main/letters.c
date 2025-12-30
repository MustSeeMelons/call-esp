#include "include/letters.h"
#include "include/display.h"

uint8_t letter_get_offset(letter_t *l) {
    return l->props & 0xf;
}

uint8_t letter_get_len(letter_t *l) {
    return (l->props >> 4) & 0xf;
}

uint8_t data_a[8] = {0, 0, 124, 18, 18, 124, 0, 0};
letter_t l_a = {.data = data_a, .props = 0x42};

uint8_t data_b[8] = {0, 0, 116, 74, 74, 126, 0, 0};
letter_t l_b = {.data = data_b, .props = 0x42};

uint8_t data_c[8] = {0, 0, 36, 66, 66, 60, 0, 0};
letter_t l_c = {.data = data_c, .props = 0x42};

uint8_t data_d[8] = {0, 0, 60, 66, 66, 126, 0, 0};
letter_t l_d = {.data = data_d, .props = 0x42};

uint8_t data_e[8] = {0, 0, 74, 74, 74, 126, 0, 0};
letter_t l_e = {.data = data_e, .props = 0x42};

uint8_t data_o[8] = {0, 0, 60, 66, 66, 60, 0, 0};
letter_t l_o = {.data = data_o, .props = 0x42};

uint8_t data_l[8] = {0, 0, 64, 64, 64, 126, 0, 0};
letter_t l_l = {.data = data_l, .props = 0x42};

uint8_t data_m[8] = {0, 126, 4, 8, 8, 4, 126, 0};
letter_t l_m = {.data = data_m, .props = 0x61};

uint8_t data_w[8] = {0, 62, 96, 16, 16, 96, 62, 0};
letter_t l_w = {.data = data_w, .props = 0x61};

letter_t *message[] = {&l_w, &l_e, &l_l, &l_c, &l_o, &l_m, &l_e};
dsp_item_t t_welcome = {.dsp_mode = DSP_TEXT_SCROLL, .data.scroll = {.count = 7, .letters = message}};
