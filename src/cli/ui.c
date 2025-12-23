#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>

#include "../../include/timer.h"
#include "../../include/display.h"

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


typedef struct {
    const char *controls;
    TimerScreenLayout *screen_layout;
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
    
    DisplayTime td = get_time_display(state->timer);
    snprintf(
        view->time_str,
        sizeof view->time_str,
        "%02d:%02d:%02d",
        td.minutes,
        td.seconds,
        td.centiseconds
    );
        
    int percent = calculate_progress(state->timer);
    progress_bar_to_buf(
        view->progress_bar,
        sizeof view->progress_bar,
        percent,
        state->screen_layout->width / 2
    );

    snprintf(
        view->category_str,
        sizeof view->category_str,
        "%s -> %s",
        state->timer->category,
        state->timer->subcategory
    );

    sprintf(view->controls, "[Space] Pause   [Q] Quit");
    view->border_color = UI_COLOR_SOFT_CYAN;


    switch (state->timer->timer_mode)
    {
    case MODE_COUNTDOWN:
        strcpy(view->header, "POMODORO TIMER");
        break;
    case MODE_STOPWATCH:
        strcpy(view->header, "STOPWATCH TIMER");
        break;
    default:
        break;
    }

    switch (state->timer->timer_state)
    {
    case STATE_ACTIVE:
        view->border_color = UI_COLOR_SOFT_CYAN;
        break;
    case STATE_PAUSED:
        view->border_color = UI_COLOR_SOFT_PURPLE;
        sprintf(view->controls, "[Space] Resume  [Q] Quit");
        break;
    case STATE_COMPLETED:
        view->border_color = UI_COLOR_SOFT_GREEN;
        break;
    case STATE_CANCELLED:
        view->border_color = UI_COLOR_SOFT_RED;
        sprintf(view->header, "CANCELLED");
        break;
    default:
        break;
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


void timer_screen_render(
    TimerScreenState *state,
    TimerScreenView *view
    ) {
    char buf[256];

    printf("\033[2J");      // Clear screen
    printf("\033[3J");      // Clear scrollback buffer
    printf("\033[H");       // Move cursor to home
    printf("\033[?25l");    // Hide cursor
    fflush(stdout);    fflush(stdout);

    // Header: top border, margin, header text, margin, lower border
    box_render_line(            //Top border
        buf,
        "",
        state->borders->top,
        state->screen_layout->width,
        view->border_color,
        1
    );
    for (int i = 0; i < state->screen_layout->padding_header_vert; i ++) {
        box_render_line(        // Margin
            buf,
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            0
        );
    }
    box_render_line(            // Header text
        buf, 
        view->header, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 
        0
    );
    for (int i = 0; i < state->screen_layout->padding_header_vert; i ++) {
        box_render_line(        // Margin
            buf,
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            0
        );
    }
    box_render_line(            // Lower border
        buf, 
        "", 
        state->borders->mid_bottom, 
        state->screen_layout->width, 
        view->border_color, 
        0
    );

    // Time: margin, time, progress bar
    for (int i = 0; i < state->screen_layout->margin_after_header; i ++) {
        box_render_line(        // Margin
            buf,
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            0
        );
    }
    box_render_line(            // Time
        buf, 
        view->time_str, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 
        0
    );
    box_render_line(            // Progress bar
        buf,    
        view->progress_bar, 
        state->borders->mid, 
        state->screen_layout->width * 2, // Specifics of rendering progress bar symbols
        view->border_color, 
        0
    );

    // Category: margin, category
    for (int i = 0; i < state->screen_layout->margin_after_time; i ++) {
        box_render_line(        // Margin      
            buf,
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            0
        );
    }
    box_render_line(            // Category 
        buf, 
        view->category_str, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 0
    );

    // Controls: margin, controls
    for (int i = 0; i < state->screen_layout->margin_after_category; i ++) {
        box_render_line(        // Margin
            buf,
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            0
        );
    }
    box_render_line(            // Controls
        buf, 
        view->controls, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 
        0
    );
    
    // Margin and bottom border
    for (int i = 0; i < state->screen_layout->margin_after_controls; i ++) {
        box_render_line(        // Margin                    
            buf,
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            0
        );
    }
    box_render_line(            // Bottom border
        buf, 
        "", 
        state->borders->bottom, 
        state->screen_layout->width, 
        view->border_color, 
        1
    );
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

    TimerScreenLayout screen_layout = {
        .width = 40,
        .padding_horizontal = 8,
        .padding_header_vert = 1,

        .margin_after_header = 3,
        .margin_after_time = 1,
        .margin_after_category = 3,
        .margin_after_controls = 1
    };

    TimerScreenState content = {
        .screen_layout = &screen_layout,
        .controls = "[Space] Pause  [Q] Quit",
        .borders = &borders,
        .timer = timer
    };

    TimerScreenView view;

    timer_screen_build_view(&content, &view);
    timer_screen_render(&content, &view);

}