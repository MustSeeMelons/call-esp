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

uint8_t data_f[8] = {0, 0, 2, 10, 10, 126, 0, 0};
letter_t l_f = {.data = data_f, .props = 0x42};

uint8_t data_g[8] = {0, 0, 52, 82, 66, 60, 0, 0};
letter_t l_g = {.data = data_g, .props = 0x42};

uint8_t data_h[8] = {0, 0, 126, 8, 8, 126, 0, 0};
letter_t l_h = {.data = data_h, .props = 0x42};

uint8_t data_k[8] = {0, 0, 98, 20, 8, 126, 0, 0};
letter_t l_k = {.data = data_k, .props = 0x42};

uint8_t data_l[8] = {0, 0, 64, 64, 64, 126, 0, 0};
letter_t l_l = {.data = data_l, .props = 0x42};

uint8_t data_m[8] = {0, 126, 4, 8, 8, 4, 126, 0};
letter_t l_m = {.data = data_m, .props = 0x61};

uint8_t data_n[8] = {0, 126, 32, 16, 8, 4, 126, 0};
letter_t l_n = {.data = data_n, .props = 0x61};

uint8_t data_i[8] = {0, 0, 0, 0, 126, 0, 0, 0};
letter_t l_i = {.data = data_i, .props = 0x13};

uint8_t data_j[8] = {0, 0, 62, 66, 66, 34, 0, 0};
letter_t l_j = {.data = data_j, .props = 0x42};

uint8_t data_o[8] = {0, 0, 60, 66, 66, 60, 0, 0};
letter_t l_o = {.data = data_o, .props = 0x42};

uint8_t data_p[8] = {0, 0, 12, 18, 18, 126, 0, 0};
letter_t l_p = {.data = data_p, .props = 0x42};

uint8_t data_q[8] = {0, 0, 60, 66, 66, 124, 64, 00};
letter_t l_q = {.data = data_q, .props = 0x51};

uint8_t data_r[8] = {0, 0, 76, 50, 18, 126, 0, 0};
letter_t l_r = {.data = data_r, .props = 0x42};

uint8_t data_s[8] = {0, 0, 36, 82, 74, 36, 0, 0};
letter_t l_s = {.data = data_s, .props = 0x42};

uint8_t data_t[8] = {0, 0, 2, 2, 126, 2, 2, 0};
letter_t l_t = {.data = data_t, .props = 0x51};

uint8_t data_u[8] = {0, 0, 62, 64, 64, 62, 0, 0};
letter_t l_u = {.data = data_u, .props = 0x42};

uint8_t data_v[8] = {0, 0, 14, 48, 64, 48, 14, 0};
letter_t l_v = {.data = data_v, .props = 0x51};

uint8_t data_w[8] = {0, 30, 32, 16, 8, 16, 32, 30};
letter_t l_w = {.data = data_w, .props = 0x70};

uint8_t data_x[8] = {0, 0, 68, 40, 16, 40, 68, 0};
letter_t l_x = {.data = data_x, .props = 0x51};

uint8_t data_y[8] = {0, 0, 6, 8, 120, 8, 6, 0};
letter_t l_y = {.data = data_y, .props = 0x51};

uint8_t data_z[8] = {0, 0, 70, 74, 82, 98, 0, 0};
letter_t l_z = {.data = data_z, .props = 0x42};

letter_t *message[] = {&l_w, &l_e, &l_l, &l_c, &l_o, &l_m, &l_e};
dsp_item_t t_welcome = {.dsp_mode = DSP_TEXT_SCROLL,
                        .data.scroll = {.count = sizeof(message) / sizeof(message[0]), .letters = message}};

letter_t *error[] = {&l_e, &l_r, &l_r, &l_o, &l_r};
dsp_item_t t_error = {.dsp_mode = DSP_TEXT_SCROLL,
                      .data.scroll = {.count = sizeof(error) / sizeof(error[0]), .letters = error}};

letter_t *gate[] = {&l_g, &l_a, &l_t, &l_e};
dsp_item_t t_gate = {.dsp_mode = DSP_TEXT_SCROLL,
                     .data.scroll = {.count = sizeof(gate) / sizeof(gate[0]), .letters = gate}};

letter_t *cool[] = {&l_c, &l_o, &l_o, &l_l};
dsp_item_t t_cool = {.dsp_mode = DSP_TEXT_SCROLL,
                     .data.scroll = {.count = sizeof(cool) / sizeof(cool[0]), .letters = cool}};
