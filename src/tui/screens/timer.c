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


#include <stdio.h>
#include <string.h>

void timer_screen_small_render(TimerScreenState *state, TimerScreenView *view) {
    // 1. Prepare Content Buffers
    char line_header[256];
    char line_body[256];
    char line_controls[256];

    const char *mode_label = (state->timer->timer_work_mode == MODE_WORK) ? "WORK" :
                             (state->timer->timer_work_mode == MODE_BREAK) ? "BREAK" : "LONG BREAK";

    // Format Line 1: Header/Time
    if (state->timer->timer_mode == MODE_COUNTDOWN && state->total_iterations >= 2) {
        snprintf(line_header, sizeof(line_header), " %s %d/%d  %s  %s",
                 mode_label, state->current_iteration, state->total_iterations,
                 view->time, view->progress_bar);
    } else {
        snprintf(line_header, sizeof(line_header), " %s  %s  %s",
                 mode_label, view->time, view->progress_bar);
    }

    // Format Line 2: Category/Activity
    snprintf(line_body, sizeof(line_body), " %s %s", view->category, view->activity);

    // Format Line 3: Controls
    snprintf(line_controls, sizeof(line_controls), " %s", view->controls);

    // 2. Determine Maximum Visible Width
    // Note: This assumes view strings don't contain invisible ANSI escape codes.
    int max_w = (int)strlen(line_header);
    int body_w = (int)strlen(line_body);
    int ctrl_w = (int)strlen(line_controls);

    if (body_w > max_w) max_w = body_w;
    if (ctrl_w > max_w) max_w = ctrl_w;

    // 3. Render
    printf("\033[2J\033[H\033[?25l"); // Clear and hide cursor

    // Top border
    printf("%s", view->border_color);
    for (int i = 0; i < max_w; i++) printf("%s", state->borders->top->mid_char);
    printf("%s\n", RESET_COLOR);

    // Content
    printf("%s\n", line_header);
    printf("%s\n", line_body);
    printf("%s\n", line_controls);

    // Bottom border
    printf("%s", view->border_color);
    for (int i = 0; i < max_w; i++) printf("%s", state->borders->top->mid_char);
    printf("%s\n", RESET_COLOR);

    fflush(stdout);
}

void timer_screen_minimal_render(
    TimerScreenState *state,
    TimerScreenView *view
) {
    printf("\033[2J\033[H\033[?25l");
    
    const char* status_color = get_state_color(state->colors, state->timer->timer_state);
    const char* sym;
    if (state->timer->timer_state == STATE_CANCELLED) sym = "[x]";
    else switch (state->timer->timer_work_mode)
    {
    case MODE_WORK:
        sym = "[#]";
        break;
    case MODE_BREAK:
        sym = "[=]";
        break;
    case MODE_LONG_BREAK:
        sym = "[+]";
        break;
    default:
        sym = "[ ]";
        break;
    }

    if (state->timer->timer_mode == MODE_COUNTDOWN && state->total_iterations >= 2) {
        printf("%s%s%s %s %d/%d │ %s │ %s %s │ %s%s\n",
               status_color, sym, RESET_COLOR,
               state->timer->timer_work_mode == MODE_WORK ? "WORK" :
               state->timer->timer_work_mode == MODE_BREAK ? "BREAK" : "LONG BREAK",
               state->current_iteration,
               state->total_iterations,
               view->time,
               view->category,
               view->activity,
               view->controls,
               RESET_COLOR);
    } else {
        printf("%s%s%s %s │ %s │ %s %s │ %s%s\n",
               status_color, sym, RESET_COLOR,
               state->timer->timer_work_mode == MODE_WORK ? "WORK" :
               state->timer->timer_work_mode == MODE_BREAK ? "BREAK" : "LONG BREAK",
               view->time,
               view->category,
               view->activity,
               view->controls,
               RESET_COLOR);
    }
    fflush(stdout);
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
        box_render_line(
            view->progress_bar, 
            state->borders->mid, 
            state->screen_layout->width * state->progress_bar->char_width,  // ← Use char_width
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