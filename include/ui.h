#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "global.h"
#include "timer.h"

#define WIDTH                   40
#define PADDING_HORIZONTAL      8
#define PADDING_HEADER_VERT     1
#define MARGIN_AFTER_HEADER     3
#define MARGIN_AFTER_TIME       1
#define MARGIN_AFTER_CATEGORY   3
#define MARGIN_AFTER_CONTROLS   1

// Colors

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

// Borders

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

// Layout, State and View

typedef struct
{
    int width;
    int padding_horizontal;
    int padding_header_vert;   
    
    int margin_after_header;
    int margin_after_time;
    int margin_after_category;
    int margin_after_controls;
} TimerScreenLayout;

typedef struct
{
    char header[HEADER_SIZE];
    char time[TIME_SIZE];
    char progress_bar[PROGRESS_BAR_SIZE];
    char category_activity[CATEGORY_ACTIVITY_SIZE];
    char controls[CONTROLS_SIZE];
    char border_color[COLOR_SIZE];
} TimerScreenView;

typedef struct {
    const TimerScreenLayout *screen_layout;
    const Timer *timer;
    const BoxBorders *borders;
    const Colors *colors;
    int current_iteration;
    int total_iterations;
} TimerScreenState;


const BoxBorders* get_borders(BorderType type);
const Colors* get_colors(ColorTheme theme);

void render_ui(
    const Timer *timer,
    const ColorTheme color_theme,
    const BorderType border_type,
    int current_iteration,
    int total_iterations
);

void timer_screen_build_view(
    TimerScreenState *state,
    TimerScreenView *view
);
void timer_screen_render(
    TimerScreenState *state,
    TimerScreenView *view
);

#endif
