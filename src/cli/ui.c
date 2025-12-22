#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>

#include "../../include/timer.h"
#include "../../include/display.h"

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

typedef struct {
    int width;
    int padding_header_vert;
    int padding_top;
    int padding_mid;
    int padding_midb;
    int padding_botttom;
    const char *controls;
    const BoxBorders *borders;
    Timer *timer;
} TimerScreenState;

typedef struct
{
    char header[64];
    char time_str[16];
    char progress_bar[128];
    char category_str[64];
    char controls[64];
    UiColor border_color;
} TimerScreenView;



// Helper functions

const char* ui_color_code(UiColor c) {
    switch (c) {
        case UI_COLOR_RED:           return "\x1b[31m";
        case UI_COLOR_GREEN:         return "\x1b[32m";
        case UI_COLOR_YELLOW:        return "\x1b[33m";
        case UI_COLOR_BLUE:          return "\x1b[34m";
        case UI_COLOR_MAGENTA:       return "\x1b[35m";
        case UI_COLOR_CYAN:          return "\x1b[36m";
        case UI_COLOR_GRAY:          return "\x1b[90m";
        
        // Gentler cyberpunk colors (using 256-color mode)
        case UI_COLOR_SOFT_CYAN:     return "\x1b[38;5;80m";   // Soft cyan-blue
        case UI_COLOR_SOFT_PURPLE:   return "\x1b[38;5;141m";  // Soft purple/lavender
        case UI_COLOR_SOFT_RED:      return "\x1b[38;5;203m";  // Soft coral red
        case UI_COLOR_SOFT_GREEN:    return "\x1b[38;5;114m";  // Soft mint green
        case UI_COLOR_NEON_PINK:     return "\x1b[38;5;213m";  // Neon pink
        case UI_COLOR_ELECTRIC_BLUE: return "\x1b[38;5;81m";   // Electric blue
        
        case UI_COLOR_DEFAULT:
        default:                     return "\x1b[0m";
    }
}

int calculate_progress(const Timer *t) {
    if (!t || t->target_ms <= 0)
        return 100;

    int64_t elapsed = get_elapsed_ms(t);

    if (elapsed <= 0)
        return 0;
    if (elapsed >= t->target_ms)
        return 100;

    return (int)((elapsed * 100) / t->target_ms);
}

static char* repeat_string(char *buf, const char *ch, int count) {
    for (int i = 0; i < count; i++) {
        strcpy(buf, ch);
        buf += strlen(ch); 
    }
    return buf;
}

// Writing values to buffer

void progress_bar_to_buf(
    char *buf,
    size_t buf_size,
    int percent,
    int width
) {

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int filled = (percent * width) / 100;

    char *p = buf;
    memset(p, ' ', 5);
    p += 5;
    size_t remaining = buf_size - 1; // leave space for '\0'

    for (int i = 0; i < width && remaining > 0; ++i) {
        const char *ch = (i < filled) ? "█" : "░";
        size_t len = strlen(ch);

        if (len > remaining)
            break;

        memcpy(p, ch, len);
        p += len;
        remaining -= len;
    }

    *p = '\0';

    char buf_percent[16];
    snprintf(buf_percent, sizeof buf_percent, " %3d%%", percent);
    strcat(p, buf_percent);
    
}


void box_line_to_buf(
    char *buf,
    const char *str,
    const Border *border,
    int width
) {
    char *p = buf;
    int slen = strlen(str);

    int total_pad = width - slen;
    int left_pad  = total_pad / 2;
    int right_pad = total_pad - left_pad;

    p = repeat_string(p, border->left_char, 1);
    p = repeat_string(p, border->mid_char, left_pad);

    memcpy(p, str, slen);
    p += slen;

    p = repeat_string(p, border->mid_char, right_pad);
    p = repeat_string(p, border->right_char, 1);

    *p = '\0';
}

// Building a view

void timer_screen_build_view(TimerScreenState *state, TimerScreenView *view) {
    if (state->timer->timer_mode == MODE_COUNTDOWN) {
        strcpy(view->header, "POMODORO TIMER");
    }
    else {
        strcpy(view->header, "STOPWATCH TIMER");
    }
    
    DisplayTime td = get_time_display(state->timer);
    snprintf(view->time_str, sizeof view->time_str,
             "%02d:%02d:%02d", td.minutes, td.seconds, td.centiseconds);

    int percent = calculate_progress(state->timer);
    progress_bar_to_buf(view->progress_bar, sizeof view->progress_bar,
                            percent, state->width / 2);

    snprintf(view->category_str, sizeof view->category_str,
            "%s -> %s", state->timer->category, state->timer->subcategory);

    if (state->timer->is_canceled)
        view->border_color = UI_COLOR_SOFT_RED;      // Gentle red
    else if (state->timer->is_paused)
        view->border_color = UI_COLOR_SOFT_PURPLE;   // Gentle purple
    else if (percent >= 100)
        view->border_color = UI_COLOR_SOFT_GREEN;    // Gentle green
    else 
        view->border_color = UI_COLOR_SOFT_CYAN;     // Gentle cyan
   
        
    if (state->timer->is_paused) {
        sprintf(view->controls, "[Space] Resume  [Q] Quit");
    }
    else {
        sprintf(view->controls, "[Space] Pause   [Q] Quit");
    }

    if (state->timer->is_canceled) {
        sprintf(view->header, "CANCELLED");
    }
}

// Rendering 

void box_render_line(
    char *buf,
    const char *str,
    const Border *border,
    int width,
    UiColor border_color,
    int paint_content  // 0 = don't color content, 1 = color content
) {
    const char *color = ui_color_code(border_color);
    
    // Print left border in color
    printf("%s%s%s", color, border->left_char, UI_COLOR_RESET);
    
    // Truncate string if too long
    char truncated[256];
    int str_len = strlen(str);
    
    if (str_len > width) {
        // Truncate and add "..."
        int max_len = width - 8; // Leave room for "..."
        if (max_len < 0) max_len = 0;
        snprintf(truncated, sizeof(truncated), "%.*s...", max_len, str);
        str = truncated;
        str_len = strlen(str);
    }
    
    // Calculate padding
    int left_pad = (width - str_len) / 2;
    int right_pad = width - str_len - left_pad;
    
    // Print middle (with or without color)
    if (paint_content) {
        printf("%s", color);
    }
    
    for (int i = 0; i < left_pad; i++) printf("%s", border->mid_char);
    printf("%s", str);
    for (int i = 0; i < right_pad; i++) printf("%s", border->mid_char);
    
    if (paint_content) {
        printf("%s", UI_COLOR_RESET);
    }
    
    // Print right border in color
    printf("%s%s%s\n", color, border->right_char, UI_COLOR_RESET);
}


void timer_screen_render(TimerScreenState *state, TimerScreenView *view) {
    char buf[256];
    int w = state->width;

    printf("\033[2J");      // Clear screen
    printf("\033[3J");      // Clear scrollback buffer
    printf("\033[H");       // Move cursor to home
    printf("\033[?25l");    // Hide cursor
    fflush(stdout);    fflush(stdout);

    // Header
    box_render_line(buf, "", state->borders->top, w, view->border_color, 1);
    for (int i = 0; i < state->padding_header_vert; i ++) {
        box_render_line(buf, "", state->borders->mid, w, view->border_color, 0);
    }
    box_render_line(buf, view->header, state->borders->mid, w, view->border_color, 0);
    for (int i = 0; i < state->padding_header_vert; i ++) {
        box_render_line(buf, "", state->borders->mid, w, view->border_color, 0);
    }
    box_render_line(buf, "", state->borders->mid_bottom, w, view->border_color, 1);

    // Time
    for (int i = 0; i < state->padding_top; i ++) {
        box_render_line(buf, "", state->borders->mid, w, view->border_color, 0);
    }

    box_render_line(buf, view->time_str, state->borders->mid, w, view->border_color, 0);
    box_render_line(buf, view->progress_bar, state->borders->mid, w * 2, view->border_color, 0);

    for (int i = 0; i < state->padding_mid; i ++) {
        box_render_line(buf, "", state->borders->mid, w, view->border_color, 0);
    }

    // Category
    box_render_line(buf, view->category_str, state->borders->mid, w, view->border_color, 0);

    for (int i = 0; i < state->padding_midb; i ++) {
        box_render_line(buf, "", state->borders->mid, w, view->border_color, 0);
    }

    // Controls
    box_render_line(buf, view->controls, state->borders->mid, w, view->border_color, 0);
    
    for (int i = 0; i < state->padding_botttom; i ++) {
        box_render_line(buf, "", state->borders->mid, w, view->border_color, 0);
    }
    
    box_render_line(buf, "", state->borders->bottom, w, view->border_color, 1);
}

void pomodoro_render(const Timer *timer) {
    const Border top_border     = {"╔", "═", "╗"};
    const Border mid_border     = {"║", " ", "║"};
    const Border midb_border    = {"╠", "═", "╣"};
    const Border bottom_border  = {"╚", "═", "╝"};

    const BoxBorders borders = {
        .top        = &top_border,
        .mid        = &mid_border,
        .mid_bottom = &midb_border,
        .bottom     = &bottom_border
    };

    TimerScreenState content = {
        .width = 40,
        .padding_header_vert = 1,
        .padding_top = 3,
        .padding_mid = 1,
        .padding_midb = 3,
        .padding_botttom = 1,
        .controls = "[Space] Pause  [Q] Quit",
        .borders = &borders,
        .timer = timer
    };

    TimerScreenView view;

    timer_screen_build_view(&content, &view);
    timer_screen_render(&content, &view);

}