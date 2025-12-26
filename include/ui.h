#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "global.h"
#include "timer.h"

typedef enum {
    THEME_MINIMAL,
    THEME_SIMPLE,
    THEME_CYBERPUNK,
    THEME_FOREST,
    THEME_SUNSET
} ColorTheme;

typedef enum {
    UI_COLOR_DEFAULT,
    UI_COLOR_RED,
    UI_COLOR_GREEN,
    UI_COLOR_YELLOW,
    UI_COLOR_BLUE,
    UI_COLOR_MAGENTA,
    UI_COLOR_CYAN,
    UI_COLOR_GRAY,

    UI_COLOR_SOFT_CYAN,
    UI_COLOR_SOFT_PURPLE,
    UI_COLOR_SOFT_RED,
    UI_COLOR_SOFT_GREEN,
    UI_COLOR_NEON_PINK,
    UI_COLOR_ELECTRIC_BLUE
} ColorCode;

typedef struct {
    const char* active;
    const char* paused;
    const char* completed;
    const char* cancelled;
} Colors;


typedef enum {
    BORDER_MINIMAL,
    BORDER_SINGLE,
    BORDER_DOUBLE
} BorderType;

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



typedef struct
{
    int width;
    int padding_horizontal;
    int padding_header_vert;    // FIX: UNUSED    
    
    int margin_after_header;
    int margin_after_time;
    int margin_after_category;
    int margin_after_controls;
} TimerScreenLayout;

typedef struct {
    TimerScreenLayout *screen_layout;
    const BoxBorders *borders;
    Timer *timer;
    int current_iteration;
    int total_iterations;
} TimerScreenState;


const BoxBorders* get_borders(BorderType type);

const char* ui_color_code(ColorCode c);

void pomodoro_render(
    const Timer *t,
    int current_iteration,
    int total_iterations
);

#endif
