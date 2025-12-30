#include "include/display.h"

uint8_t blank[8] = {0, 0, 0, 0, 0, 0, 0, 0};

uint8_t circle_1[8] = {0, 0, 0, 16, 0, 0, 0, 0};
uint8_t circle_2[8] = {0, 0, 0, 0, 16, 0, 0, 0};
uint8_t circle_3[8] = {0, 0, 0, 0, 8, 0, 0, 0};
uint8_t circle_4[8] = {0, 0, 0, 8, 0, 0, 0, 0};

uint8_t *circle_data[] = {circle_1, circle_2, circle_3, circle_4};
dsp_anim_data circle = {.count = 4, .frame = circle_data};

uint8_t warning_1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t warning_2[8] = {0, 0, 0, 223, 223, 0, 0, 0};

uint8_t *warning_data[] = {warning_1, warning_1, warning_1, warning_2, warning_2, warning_2};
dsp_anim_data warning = {.count = 6, .frame = warning_data};

uint8_t ball_1[8] = {255, 129, 129, 129, 129, 129, 129, 255};
uint8_t ball_2[8] = {0, 126, 66, 66, 66, 66, 126, 0};
uint8_t ball_3[8] = {0, 0, 60, 36, 36, 60, 0, 0};
uint8_t ball_4[8] = {0, 0, 0, 24, 24, 0, 0, 0};

uint8_t *ball_data[] = {ball_1, ball_1, ball_2, ball_3, ball_4, ball_4, ball_4, ball_3, ball_2, ball_1, ball_1};
dsp_anim_data ball = {.count = 11, .frame = ball_data};
