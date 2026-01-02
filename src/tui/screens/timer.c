#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../../include/timer.h"
#include "../../../include/ui/ui.h"
#include "../../../include/global.h"
#include "../../../include/literals.h"

// Rendering 


void timer_screen_minimal_render(void);


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
    for (int i = 0; i < state->screen_layout->margin_after_time; i++) {
        box_render_line(
            "",
            state->borders->mid, 
            state->screen_layout->width, 
            view->border_color, 
            state->screen_layout->padding_horizontal, 
            0
        );
    }
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