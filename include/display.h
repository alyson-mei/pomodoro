#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"

#define UI_COLOR_RESET "\x1b[0m"

typedef enum {
    UI_COLOR_DEFAULT,
    UI_COLOR_RED,
    UI_COLOR_GREEN,
    UI_COLOR_YELLOW,
    UI_COLOR_BLUE,
    UI_COLOR_MAGENTA,
    UI_COLOR_CYAN,
    UI_COLOR_GRAY,
    UI_COLOR_SOFT_CYAN,      // Gentle cyan for running
    UI_COLOR_SOFT_PURPLE,    // Soft purple for paused
    UI_COLOR_SOFT_RED,       // Soft red for cancelled
    UI_COLOR_SOFT_GREEN,     // Soft green for completed
    UI_COLOR_NEON_PINK,      // Cyberpunk accent
    UI_COLOR_ELECTRIC_BLUE   // Cyberpunk accent
} UiColor;

typedef struct {
    const char *left_char;
    const char *mid_char;
    const char *right_char;
} Border;

typedef struct {
    const Border *top;
    const Border *mid;
    const Border *mid_bottom;
    const Border *bottom;
} BoxBorders;

const char* ui_color_code(UiColor c);
void pomodoro_render(const Timer *t);

#endif
