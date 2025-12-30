#include "esp_err.h"
#include "include/letters.h"

// Runtime scrollable text data
typedef struct {
    letter_t **letters;
    size_t count;
    int scroll_pos;
} scroll_text_t;

// Runtime animation data
typedef struct {
    uint8_t **frame;
    size_t count;
    int active;
} anim_t;

// Logic path how to handle the display
typedef enum
{
    DSP_IDLE = 0,
    DSP_TEST = 1,
    DSP_FRAME_ANIM = 2,
    DSP_TEXT_SCROLL = 3
} dsp_mode_t;

typedef struct {
    letter_t **letters;
    size_t count;
} dsp_scroll_data;

typedef struct {
    uint8_t **frame;
    size_t count;
} dsp_anim_data;

typedef struct {
    dsp_mode_t dsp_mode;
    union {
        dsp_scroll_data scroll;
        dsp_anim_data anim;
    } data;
} dsp_item_t;

esp_err_t display_init();

void display_control_task_start();

void display_run_task_start();

void display_ticker_task_start();

void display_cmd_send(uint8_t message);