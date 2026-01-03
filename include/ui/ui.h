#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "global.h"
#include "timer.h"

#define WIDTH                   50
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
    const char *category;
    const char *activity;
    const char *controls;
    const char *border_color;
} TimerScreenView;

typedef struct {
    const TimerScreenLayout *screen_layout;
    const Timer *timer;
    const BoxBorders *borders;
    const Colors *colors;
    const char *category;
    const char *activity;
    int current_iteration;
    int total_iterations;
} TimerScreenState;

typedef struct {
    TimerScreenLayout layout;
    const BoxBorders* borders;
    const Colors* colors;
    const char* category;
    const char* activity;
} UIConfig;

const BoxBorders* get_borders(BorderType type);
const Colors* get_colors(ColorTheme theme);


void render_ui(
    const UIConfig *config,
    const Timer *timer,
    int current_iteration,
    int total_iterations
);

void timer_screen_build_view(
    TimerScreenState *state,
    TimerScreenView *view
);
void timer_screen_balanced_render(
    TimerScreenState *state,
    TimerScreenView *view
);
void timer_screen_small_render(
    TimerScreenState *state,
    TimerScreenView *view
);
void timer_screen_minimal_render(
    TimerScreenState *state,
    TimerScreenView *view
);

// TODO: refactor this later!


void set_header(
    char *header,
    size_t header_size,
    TimerMode mode, 
    TimerWorkMode work_mode,
    TimerState state,
    int current_iteration,
    int total_iterations
);

const char* get_controls_text(TimerState state);
const char* get_state_color(const Colors *colors, TimerState state);

int calculate_progress(const Timer *t);
char* repeat_string(
    char *buf,
    const char *ch, 
    int count
);

void format_progress_bar(
    char *buf,
    size_t buf_size,
    int percent,
    int width
);



void box_line_to_buf(
    char *buf,
    const char *str,
    const Border *border,
    int width
);

void box_render_line(
    const char *str,
    const Border *border,
    int width,
    const char* color,
    int padding_horizontal,
    int paint_content         // 0 = don't color content, 1 = color content
);


//



#endif
