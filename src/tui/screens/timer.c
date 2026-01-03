#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../../include/timer.h"
#include "../../../include/ui/ui.h"
#include "../../../include/global.h"
#include "../../../include/literals.h"

// Rendering 

static inline void render_margin_lines(
    const TimerScreenState *state,
    const char *border_color,
    int line_count
) {
    for (int i = 0; i < line_count; i++) {
        box_render_line(
            "",
            state->borders->mid,
            state->screen_layout->width,
            border_color,
            state->screen_layout->padding_horizontal,
            0
        );
    }
}


void timer_screen_small_render(
    TimerScreenState *state,
    TimerScreenView *view
) {
    printf("\033[2J\033[H\033[?25l");
    fflush(stdout);
    
    // Top border
    printf("%s", view->border_color);
    for (int i = 0; i < state->screen_layout->width; i++) {
        printf("━");
    }
    printf("%s\n", UI_COLOR_RESET);
    
    // Header and time line
    if (state->timer->timer_mode == MODE_COUNTDOWN && state->total_iterations >= 2) {
        printf(" %s %d/%d  %s  %s\n",
               state->timer->timer_work_mode == MODE_WORK ? "WORK" :
               state->timer->timer_work_mode == MODE_BREAK ? "BREAK" : "LONG BREAK",
               state->current_iteration,
               state->total_iterations,
               view->time,
               view->progress_bar);
    } else {
        printf(" %s  %s  %s\n",
               state->timer->timer_work_mode == MODE_WORK ? "WORK" :
               state->timer->timer_work_mode == MODE_BREAK ? "BREAK" : "LONG BREAK",
               view->time,
               view->progress_bar);
    }
    
    // Category and activity
    printf(" %s %s\n", view->category, view->activity);
    
    // Controls
    printf(" %s\n", view->controls);
    
    // Bottom border
    printf("%s", view->border_color);
    for (int i = 0; i < state->screen_layout->width; i++) {
        printf("━");
    }
    printf("%s\n", UI_COLOR_RESET);
}

void timer_screen_minimal_render(
    TimerScreenState *state,
    TimerScreenView *view
) {
    printf("\033[2J\033[H\033[?25l");
    fflush(stdout);
    
    // Single line format
    if (state->timer->timer_mode == MODE_COUNTDOWN && state->total_iterations >= 2) {
        printf("%s %d/%d | %s │ %s %s │ %s%s\n",
               state->timer->timer_work_mode == MODE_WORK ? "WORK" :
               state->timer->timer_work_mode == MODE_BREAK ? "BREAK" : "LONG BREAK",
               state->current_iteration,
               state->total_iterations,
               view->time,
               view->category,
               view->activity,
               view->controls,
               UI_COLOR_RESET);
    } else {
        printf("%s │ %s │ %s %s │ %s%s\n",
               state->timer->timer_work_mode == MODE_WORK ? "WORK" :
               state->timer->timer_work_mode == MODE_BREAK ? "BREAK" : "LONG BREAK",
               view->time,
               view->category,
               view->activity,
               view->controls,
               UI_COLOR_RESET);
    }
}

void timer_screen_balanced_render(
    TimerScreenState *state,
    TimerScreenView *view
    ) {

    printf("\033[2J");      // Clear screen
    printf("\033[3J");      // Clear scrollback buffer
    printf("\033[H");       // Move cursor to home
    printf("\033[?25l");    // Hide cursor
    fflush(stdout);    fflush(stdout);

    // Header: top border, margin, header text, margin, lower border
    box_render_line(            // Top border 
        "", 
        state->borders->top, 
        state->screen_layout->width, 
        view->border_color,
        state->screen_layout->padding_horizontal, 
        1
    );
    render_margin_lines(        // Margin 
        state, 
        view->border_color, 
        state->screen_layout->padding_header_vert
    ); 
    box_render_line(            // Header text
        view->header, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color,
        state->screen_layout->padding_horizontal, 
        0
    );
    render_margin_lines(        // Margin 
        state, 
        view->border_color, 
        state->screen_layout->padding_header_vert
    ); 
    box_render_line(            // Mid-bottom border 
        "", 
        state->borders->mid_bottom, 
        state->screen_layout->width, 
        view->border_color,
        state->screen_layout->padding_horizontal, 
        1
    );

    // Time: margin, time, progress bar
    render_margin_lines(        // Margin 
        state, 
        view->border_color, 
        state->screen_layout->margin_after_header
    ); 
    box_render_line(            // Time
        view->time, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 
        state->screen_layout->padding_horizontal, 
        0
    );
    if (state->timer->timer_mode == MODE_COUNTDOWN) {
        box_render_line(                     // Progress bar
            view->progress_bar, 
            state->borders->mid, 
            state->screen_layout->width * 2, // Specifics of rendering progress bar symbols
            view->border_color, 
            state->screen_layout->padding_horizontal, 
            0
        );
    }

    // Category: margin, category, activity
    render_margin_lines(        // Margin 
        state, 
        view->border_color, 
        state->screen_layout->margin_after_time
    ); 
    box_render_line(            // Category with comma
        view->category, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color,
        state->screen_layout->padding_horizontal,  
        0
    );
    box_render_line(            // Activity (indented feel)
        view->activity, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color,
        state->screen_layout->padding_horizontal,  
        0
    );

    // Controls: margin, controls
    render_margin_lines(        // Margin 
        state, 
        view->border_color, 
        state->screen_layout->margin_after_category
    ); 
    box_render_line(            // Controls
        view->controls, 
        state->borders->mid, 
        state->screen_layout->width, 
        view->border_color, 
        state->screen_layout->padding_horizontal,         
        0
    );
    
    // Margin and bottom border
    render_margin_lines(        // Margin 
        state, 
        view->border_color, 
        state->screen_layout->margin_after_controls
    ); 
    box_render_line(            // Bottom border
        "", 
        state->borders->bottom, 
        state->screen_layout->width, 
        view->border_color, 
        state->screen_layout->padding_horizontal,
        1
    );
}