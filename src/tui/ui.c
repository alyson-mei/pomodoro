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
        "%02d:%02d:%02d",
        td.minutes,
        td.seconds,
        td.centiseconds
    );
        
    int percent = calculate_progress(state->timer);
    format_progress_bar(
        view->progress_bar,
        sizeof view->progress_bar,
        percent,
        state->screen_layout->width / 2
    );

    snprintf(
        view->category,
        sizeof view->category,
        "%s,",
        state->timer->category
    );

    snprintf(
        view->activity,
        sizeof view->activity,
        "%s",
        state->timer->activity
    );

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


void render_ui(
    const Timer *timer,
    const ColorTheme color_theme,
    const BorderType border_type,
    int current_iteration,
    int total_iterations
) {    
    
    const BoxBorders* borders = get_borders(border_type);
    const Colors* colors = get_colors(color_theme);

    TimerScreenLayout screen_layout = {
        .width = WIDTH,
        .padding_horizontal = PADDING_HORIZONTAL,
        .padding_header_vert = PADDING_HEADER_VERT,

        .margin_after_header = MARGIN_AFTER_HEADER,
        .margin_after_time = MARGIN_AFTER_TIME,
        .margin_after_category = MARGIN_AFTER_CATEGORY,
        .margin_after_controls = MARGIN_AFTER_CONTROLS
    };

    TimerScreenState state = {
        .screen_layout = &screen_layout,
        .borders = borders,
        .colors = colors,
        .timer = timer,
        .current_iteration = current_iteration,
        .total_iterations = total_iterations
    };

    TimerScreenView view;

    timer_screen_build_view(&state, &view);
    timer_screen_balanced_render(&state, &view);

}