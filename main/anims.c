#include "include/display.h"

uint8_t blank[8] = {0, 0, 0, 0, 0, 0, 0, 0};

uint8_t breathe_1[8] = {255, 129, 129, 129, 129, 129, 129, 255};
uint8_t breathe_2[8] = {0, 126, 66, 66, 66, 66, 126, 0};
uint8_t breathe_3[8] = {0, 0, 60, 36, 36, 60, 0, 0};
uint8_t breathe_4[8] = {0, 0, 0, 24, 24, 0, 0, 0};

uint8_t *breathe_data[] = {breathe_1,
                           breathe_1,
                           breathe_2,
                           breathe_3,
                           breathe_4,
                           breathe_4,
                           breathe_4,
                           breathe_3,
                           breathe_2,
                           breathe_1,
                           breathe_1};
dsp_anim_data breathe = {.count = sizeof(breathe_data) / sizeof(breathe_data[0]), .frame = breathe_data};

uint8_t circle_ball_1[8] = {126, 129, 129, 129, 129, 129, 129, 126};
uint8_t circle_ball_2[8] = {0, 60, 66, 66, 66, 66, 60, 0};
uint8_t circle_ball_3[8] = {0, 0, 24, 36, 36, 24, 0, 0};
uint8_t circle_ball_4[8] = {0, 0, 0, 24, 24, 0, 0, 0};

uint8_t *circle_ball_data[] = {circle_ball_1, circle_ball_2, circle_ball_3, circle_ball_4};
dsp_anim_data circle_ball = {.count = 4, .frame = circle_ball_data};

// Wave Thing
uint8_t wave_1[8] = {128, 64, 32, 16, 8, 4, 2, 1};
uint8_t wave_2[8] = {0, 192, 32, 16, 8, 4, 3, 0};
uint8_t wave_3[8] = {0, 0, 224, 16, 8, 7, 0, 0};
uint8_t wave_4[8] = {0, 0, 0, 240, 15, 0, 0, 0};
uint8_t wave_5[8] = {0, 0, 0, 15, 240, 0, 0, 0};
uint8_t wave_6[8] = {0, 0, 7, 8, 16, 224, 0, 0};
uint8_t wave_7[8] = {0, 3, 4, 8, 16, 32, 192, 0};
uint8_t wave_8[8] = {1, 2, 4, 8, 16, 32, 64, 128};

uint8_t *wave_data[] = {wave_1,
                        wave_2,
                        wave_3,
                        wave_4,
                        wave_5,
                        wave_6,
                        wave_7,
                        wave_8,
                        wave_7,
                        wave_6,
                        wave_5,
                        wave_4,
                        wave_3,
                        wave_2,
                        wave_1};
dsp_anim_data wave = {.count = 15, .frame = wave_data};

uint8_t line_1[8] = {1, 1, 1, 1, 1, 1, 1, 1};
uint8_t line_2[8] = {1, 1, 1, 1, 1, 1, 1, 2};
uint8_t line_3[8] = {1, 1, 1, 1, 1, 1, 2, 4};
uint8_t line_5[8] = {1, 1, 1, 1, 1, 2, 4, 8};
uint8_t line_6[8] = {1, 1, 1, 1, 2, 4, 8, 16};
uint8_t line_7[8] = {1, 1, 1, 2, 4, 8, 16, 32};
uint8_t line_8[8] = {1, 1, 2, 4, 8, 16, 32, 64};
uint8_t line_9[8] = {1, 2, 4, 8, 16, 32, 64, 128};
uint8_t line_10[8] = {2, 4, 8, 16, 32, 64, 128, 128};
uint8_t line_11[8] = {4, 8, 16, 32, 64, 128, 128, 128};
uint8_t line_12[8] = {8, 16, 32, 64, 128, 128, 128, 128};
uint8_t line_13[8] = {16, 32, 64, 128, 128, 128, 128, 128};
uint8_t line_14[8] = {32, 64, 128, 128, 128, 128, 128, 128};
uint8_t line_15[8] = {64, 128, 128, 128, 128, 128, 128, 128};
uint8_t line_16[8] = {128, 128, 128, 128, 128, 128, 128, 128};

uint8_t *line_data[] = {line_1,  line_2,  line_2,  line_3,  line_3,  line_5,  line_6,  line_7,  line_8,
                        line_9,  line_10, line_11, line_12, line_13, line_14, line_15, line_16, line_16,
                        line_15, line_15, line_14, line_14, line_13, line_13, line_12, line_11, line_10,
                        line_9,  line_8,  line_7,  line_6,  line_5,  line_3,  line_2,  line_2,  line_1};
dsp_anim_data line = {.count = sizeof(line_data) / sizeof(line_data[0]), .frame = line_data};

uint8_t sync_line_1[8] = {1, 1, 1, 1, 1, 1, 1, 1};
uint8_t sync_line_2[8] = {1, 1, 1, 1, 1, 1, 1, 2};
uint8_t sync_line_3[8] = {1, 1, 1, 1, 1, 1, 2, 4};
uint8_t sync_line_4[8] = {1, 1, 1, 1, 1, 2, 4, 8};
uint8_t sync_line_5[8] = {1, 1, 1, 1, 2, 4, 8, 16};
uint8_t sync_line_6[8] = {1, 1, 1, 2, 4, 8, 16, 32};
uint8_t sync_line_7[8] = {1, 1, 2, 4, 8, 16, 32, 64};
uint8_t sync_line_8[8] = {2, 4, 8, 16, 32, 64, 128, 128};
uint8_t sync_line_9[8] = {4, 8, 16, 32, 64, 128, 128, 128};
uint8_t sync_line_10[8] = {8, 16, 32, 64, 128, 128, 128, 128};
uint8_t sync_line_11[8] = {16, 32, 64, 128, 128, 128, 128, 128};
uint8_t sync_line_12[8] = {32, 64, 128, 128, 128, 128, 128, 128};
uint8_t sync_line_13[8] = {64, 128, 128, 128, 128, 128, 128, 128};
uint8_t sync_line_14[8] = {128, 128, 128, 128, 128, 128, 128, 128};
uint8_t sync_line_15[8] = {128, 128, 128, 128, 128, 128, 128, 64};
uint8_t sync_line_16[8] = {128, 128, 128, 128, 128, 128, 64, 32};
uint8_t sync_line_17[8] = {128, 128, 128, 128, 128, 64, 32, 16};
uint8_t sync_line_18[8] = {128, 128, 128, 128, 64, 32, 16, 8};
uint8_t sync_line_19[8] = {128, 128, 128, 64, 32, 16, 8, 4};
uint8_t sync_line_20[8] = {128, 128, 64, 32, 16, 8, 4, 2};
uint8_t sync_line_21[8] = {128, 64, 32, 16, 8, 4, 2, 1};
uint8_t sync_line_22[8] = {64, 32, 16, 8, 4, 2, 1, 1};
uint8_t sync_line_23[8] = {32, 16, 8, 4, 2, 1, 1, 1};
uint8_t sync_line_24[8] = {16, 8, 4, 2, 1, 1, 1, 1};
uint8_t sync_line_25[8] = {8, 4, 2, 1, 1, 1, 1, 1};
uint8_t sync_line_26[8] = {4, 2, 1, 1, 1, 1, 1, 1};
uint8_t sync_line_27[8] = {2, 1, 1, 1, 1, 1, 1, 1};
uint8_t sync_line_28[8] = {1, 1, 1, 1, 1, 1, 1, 1};

uint8_t *sync_line_data[] = {sync_line_2,  sync_line_3,  sync_line_4,  sync_line_5,  sync_line_6,
                             sync_line_7,  sync_line_8,  sync_line_9,  sync_line_10, sync_line_11,
                             sync_line_12, sync_line_13, sync_line_15, sync_line_16, sync_line_17,
                             sync_line_18, sync_line_19, sync_line_20, sync_line_21, sync_line_22,
                             sync_line_23, sync_line_24, sync_line_25, sync_line_26, sync_line_27};

dsp_anim_data sync_line = {.count = sizeof(sync_line_data) / sizeof(sync_line_data[0]), .frame = sync_line_data};

uint8_t load_1[8] = {3, 0, 0, 0, 0, 0, 0, 0};
uint8_t load_2[8] = {12, 0, 0, 0, 0, 0, 0, 0};
uint8_t load_3[8] = {48, 0, 0, 0, 0, 0, 0, 0};
uint8_t load_4[8] = {192, 0, 0, 0, 0, 0, 0, 0};
uint8_t load_5[8] = {0, 128, 128, 0, 0, 0, 0, 0};
uint8_t load_6[8] = {0, 0, 0, 128, 128, 0, 0, 0};
uint8_t load_7[8] = {0, 0, 0, 0, 0, 128, 128, 0};
uint8_t load_8[8] = {0, 0, 0, 0, 0, 0, 0, 192};
uint8_t load_9[8] = {0, 0, 0, 0, 0, 0, 0, 48};
uint8_t load_10[8] = {0, 0, 0, 0, 0, 0, 0, 12};
uint8_t load_11[8] = {0, 0, 0, 0, 0, 0, 0, 3};
uint8_t load_12[8] = {0, 0, 0, 0, 0, 1, 1, 0};
uint8_t load_13[8] = {0, 0, 0, 1, 1, 0, 0, 0};
uint8_t load_14[8] = {0, 1, 1, 0, 0, 0, 0, 0};

uint8_t *load_data[] = {load_1,
                        load_2,
                        load_3,
                        load_4,
                        load_5,
                        load_6,
                        load_7,
                        load_8,
                        load_9,
                        load_10,
                        load_11,
                        load_12,
                        load_13,
                        load_14};

dsp_anim_data load = {.count = sizeof(load_data) / sizeof(load_data[0]), .frame = load_data};

uint8_t standby_1[8] = {255, 0, 0, 0, 0, 0, 0, 0};
uint8_t standby_2[8] = {255, 255, 0, 0, 0, 0, 0, 0};
uint8_t standby_3[8] = {255, 255, 255, 0, 0, 0, 0, 0};
uint8_t standby_4[8] = {255, 255, 255, 255, 0, 0, 0, 0};
uint8_t standby_5[8] = {255, 255, 255, 255, 255, 0, 0, 0};
uint8_t standby_6[8] = {255, 255, 255, 255, 255, 255, 0, 0};
uint8_t standby_7[8] = {255, 255, 255, 255, 255, 255, 255, 0};
uint8_t standby_8[8] = {255, 255, 255, 255, 255, 255, 255, 255};
uint8_t standby_9[8] = {0, 255, 255, 255, 255, 255, 255, 255};
uint8_t standby_10[8] = {0, 0, 255, 255, 255, 255, 255, 255};
uint8_t standby_11[8] = {0, 0, 0, 255, 255, 255, 255, 255};
uint8_t standby_12[8] = {0, 0, 0, 0, 255, 255, 255, 255};
uint8_t standby_13[8] = {0, 0, 0, 0, 0, 255, 255, 255};
uint8_t standby_14[8] = {0, 0, 0, 0, 0, 0, 255, 255};
uint8_t standby_15[8] = {0, 0, 0, 0, 0, 0, 0, 255};

uint8_t *standby_data[] = {blank,
                           standby_1,
                           standby_2,
                           standby_3,
                           standby_4,
                           standby_5,
                           standby_6,
                           standby_7,
                           standby_8,
                           standby_9,
                           standby_10,
                           standby_11,
                           standby_12,
                           standby_13,
                           standby_14,
                           standby_15,
                           blank};

dsp_anim_data standby = {.count = sizeof(standby_data) / sizeof(standby_data[0]), .frame = standby_data};

uint8_t duck_1[8] = {0, 0, 0, 0, 0, 0, 0, 52};
uint8_t duck_2[8] = {0, 0, 0, 0, 0, 0, 52, 254};
uint8_t duck_3[8] = {0, 0, 0, 0, 0, 52, 254, 58};
uint8_t duck_4[8] = {0, 0, 0, 0, 52, 254, 58, 254};
uint8_t duck_5[8] = {0, 0, 0, 52, 254, 58, 254, 48};
uint8_t duck_6[8] = {0, 0, 52, 254, 58, 254, 48, 0};
uint8_t duck_7[8] = {0, 52, 254, 58, 254, 48, 0, 0};
uint8_t duck_8[8] = {52, 254, 58, 254, 48, 0, 0, 0};
uint8_t duck_9[8] = {254, 58, 254, 48, 0, 0, 0, 0};
uint8_t duck_10[8] = {58, 254, 48, 0, 0, 0, 0, 0};
uint8_t duck_11[8] = {254, 48, 0, 0, 0, 0, 0, 0};
uint8_t duck_12[8] = {48, 0, 0, 0, 0, 0, 0, 0};

uint8_t *duck_data[] = {
    blank, duck_1, duck_2, duck_3, duck_4, duck_5, duck_6, duck_7, duck_8, duck_9, duck_10, duck_11, duck_12, blank};

dsp_anim_data duck = {.count = sizeof(duck_data) / sizeof(duck_data[0]), .frame = duck_data};
