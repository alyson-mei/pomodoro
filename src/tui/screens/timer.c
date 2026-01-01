#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../../include/timer.h"
#include "../../../include/ui/ui.h"
#include "../../../include/global.h"
#include "../../../include/literals.h"


// Timer screen specific display functions

void set_header(
    char *header,
    TimerMode mode, 
    TimerWorkMode work_mode,
    TimerState state,
    int current_iteration,
    int total_iterations
) {
    const char *mode_str = (mode == MODE_COUNTDOWN) ? TIMER_COUNTDOWN : TIMER_STOPWATCH;
    
    if (state == STATE_CANCELLED) {
        if (mode == MODE_COUNTDOWN && total_iterations >= 2) {
            sprintf(header, "%s: %s (%d / %d)", mode_str, TIMER_CANCELLED, current_iteration, total_iterations);
        } else {
            sprintf(header, "%s: %s", mode_str, TIMER_CANCELLED);
        }
    }

    else {
        const char *work_str;
        switch (work_mode) {
            case MODE_WORK:       work_str = TIMER_WORK; break;
            case MODE_BREAK:      work_str = TIMER_BREAK; break;
            case MODE_LONG_BREAK: work_str = TIMER_LONG_BREAK; break;
            default:              work_str = NULL; break;
        }
        
        if (work_str) {
            if (total_iterations >= 2) {
                sprintf(header, "%s: %s (%d / %d)", mode_str, work_str, current_iteration, total_iterations);
            } else {
                sprintf(header, "%s: %s", mode_str, work_str);
            }
        } else {
            sprintf(header, "%s", mode_str);
        }
    }
}

void set_controls(
    char* controls,
    TimerState state
) {
    switch (state) {
        case STATE_ACTIVE:          
            sprintf(controls, CONTROLS_ACTIVE);
            break;
        case STATE_PAUSED:          
            sprintf(controls, CONTROLS_PAUSED);
            break;
        case STATE_COMPLETED:       
            sprintf(controls, CONTROLS_COMPLETED);
            break;
        case STATE_CANCELLED:
            sprintf(controls, CONTROLS_CANCELLED);
            break;
        default:
            sprintf(controls, " ");
            break;
    }
}

void set_color(
    char* color,
    const Colors *colors,
    TimerState state) {
    switch (state) {
        case STATE_ACTIVE:          
            sprintf(color, colors->active);
            break;
        case STATE_PAUSED:          
            sprintf(color, colors->paused);
            break;
        case STATE_COMPLETED:       
            sprintf(color, colors->completed);
            break;
        case STATE_CANCELLED:
            sprintf(color, colors->cancelled);
            break;
        default:
            sprintf(color, colors->active);
            break;
    }
}

// Helper functions

int calculate_progress(const Timer *t) {
    if (!t || t->target_ms <= 0)
        return 100;

    // Always show 100% when completed
    if (t->timer_state == STATE_COMPLETED)
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

    char buf_percent[BUF_PERCENT_SIZE];
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

void timer_screen_build_view(
    TimerScreenState *state,
    TimerScreenView *view
) {
    
    TimerDisplay td = get_time_display(state->timer);
    snprintf(
        view->time,
        sizeof view->time,
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

    int written = snprintf(                     //TODO: FIX TRUNCATION, SIZE LOGIC
        view->category_activity,
        sizeof view->category_activity,
        "%s -> %s",
        state->timer->category,
        state->timer->activity
    );

    // Optionally handle truncation
    if (written >= (int)(sizeof view->category_activity)) {
        // String was truncated, maybe add "..." at the end
        strcpy(view->category_activity + sizeof(view->category_activity) - 4, "...");
    }

    set_header(
        view->header,
        state->timer->timer_mode,
        state->timer->timer_work_mode,
        state->timer->timer_state,
        state->current_iteration,                                  
        state->total_iterations                         
    );

    set_controls(
        view->controls, 
        state->timer->timer_state
    );

    set_color(
        view->border_color,
        state->colors, 
        state->timer->timer_state
    );

}

// Rendering 

void box_render_line(
    const char *str,
    const Border *border,
    int width,
    const char* color,
    int padding_horizontal,
    int paint_content         // 0 = don't color content, 1 = color content
) {
    
    bool special_case = strcmp(border->left_char, "-") == 0 && strcmp(border->right_char, "-") == 0;

    // Print left border in color
    if (!special_case)
    printf("%s%s%s", color, border->left_char, UI_COLOR_RESET);
    else printf(" ");
    
    // Truncate string if too long
    char truncated[COMMON_STR_SIZE];
    int str_len = strlen(str);
    
    if (str_len > width) {
        // Truncate and add "..."
        int max_len = width - padding_horizontal; // Leave room for "..."
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
    if (!special_case)
    printf("%s%s%s\n", color, border->right_char, UI_COLOR_RESET);
    else printf(" \n");
}


void timer_screen_render(
    TimerScreenState *state,
    TimerScreenView *view
    ) {

    printf("\033[2J");      // Clear screen
    printf("\033[3J");      // Clear scrollback buffer
    printf("\033[H");       // Move cursor to home
    printf("\033[?25l");    // Hide cursor
    fflush(stdout);    fflush(stdout);

    // Header: top border, margin, header text, margin, lower border
    box_render_line(            //Top border
        "",
        state->borders->top,
        state->screen_layout->width,
        view->border_color,
        state->screen_layout->padding_horizontal,
        1
    );
    for (int i = 0; i < state->screen_layout->padding_header_vert; i ++) {
        box_render_line(        // Margin
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            state->screen_layout->padding_horizontal,
            0
        );
    }
    box_render_line(            // Header text
        view->header, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color,
        state->screen_layout->padding_horizontal, 
        0
    );
    for (int i = 0; i < state->screen_layout->padding_header_vert; i ++) {
        box_render_line(        // Margin
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color,
            state->screen_layout->padding_horizontal, 
            0
        );
    }
    box_render_line(            // Lower border
        "", 
        state->borders->mid_bottom, 
        state->screen_layout->width, 
        view->border_color, 
        state->screen_layout->padding_horizontal, 
        1
    );

    // Time: margin, time, progress bar
    for (int i = 0; i < state->screen_layout->margin_after_header; i ++) {
        box_render_line(        // Margin
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            state->screen_layout->padding_horizontal, 
            0
        );
    }
    box_render_line(            // Time
        view->time, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 
        state->screen_layout->padding_horizontal, 
        0
    );
    box_render_line(            // Progress bar
        view->progress_bar, 
        state->borders->mid, 
        state->screen_layout->width * 2, // Specifics of rendering progress bar symbols
        view->border_color, 
        state->screen_layout->padding_horizontal, 
        0
    );

    // Category: margin, category
    for (int i = 0; i < state->screen_layout->margin_after_time; i ++) {
        box_render_line(        // Margin      
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            state->screen_layout->padding_horizontal, 
            0
        );
    }
    box_render_line(            // Category 
        view->category_activity, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color,
        state->screen_layout->padding_horizontal,  
        0
    );

    // Controls: margin, controls
    for (int i = 0; i < state->screen_layout->margin_after_category; i ++) {
        box_render_line(        // Margin
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            state->screen_layout->padding_horizontal, 
            0
        );
    }
    box_render_line(            // Controls
        view->controls, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 
        state->screen_layout->padding_horizontal,         
        0
    );
    
    // Margin and bottom border
    for (int i = 0; i < state->screen_layout->margin_after_controls; i ++) {
        box_render_line(        // Margin                    
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            state->screen_layout->padding_horizontal, 
            0
        );
    }
    box_render_line(            // Bottom border
        "", 
        state->borders->bottom, 
        state->screen_layout->width, 
        view->border_color, 
        state->screen_layout->padding_horizontal,
        1
    );
}