#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../include/ui/ui.h"
#include "../../include/timer.h"
#include "../../include/global.h"
#include "../../include/literals.h"

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
    timer_screen_render(&state, &view);

}