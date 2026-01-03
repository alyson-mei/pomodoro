#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../include/ui/ui.h"
#include "../../include/timer.h"
#include "../../include/global.h"
#include "../../include/literals.h"

void timer_screen_build_view(
    TimerScreenState *state,
    TimerScreenView *view
) {
    
    TimerDisplay td = get_time_display(state->timer);
    snprintf(
        view->time,
        sizeof view->time,
        "%02d:%02d:%1d",
        td.minutes,
        td.seconds,
        td.deciseconds
    );
        
    int percent = calculate_progress(state->timer);
    format_progress_bar(
        view->progress_bar,
        sizeof view->progress_bar,
        percent,
        state->screen_layout->width / 2
    );

    view->category = state->category;
    view->activity = state->activity;

    set_header(
        view->header,
        HEADER_SIZE,
        state->timer->timer_mode,
        state->timer->timer_work_mode,
        state->timer->timer_state,
        state->current_iteration,                                  
        state->total_iterations                         
    );



    view->border_color = get_state_color(state->colors, state->timer->timer_state);
    view->controls = get_controls_text(state->timer->timer_state);


}


void render_ui(
    const UIConfig *config,
    const Timer *timer,
    int current_iteration,
    int total_iterations
) {
    TimerScreenState state = {
        .screen_layout = &config->layout,
        .borders = config->borders,
        .colors = config->colors,
        .timer = timer,
        .category = config->category,
        .activity = config->activity,
        .current_iteration = current_iteration,
        .total_iterations = total_iterations
    };

    TimerScreenView view;
    timer_screen_build_view(&state, &view);
    timer_screen_small_render(&state, &view);
}
