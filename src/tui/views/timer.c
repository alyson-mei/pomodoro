#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../../include/timer.h"
#include "../../../include/ui/ui.h"
#include "../../../include/global.h"
#include "../../../include/literals.h"


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
            if (total_iterations >= 2 && mode == MODE_COUNTDOWN) {
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


void format_progress_bar(
    char *buf,
    size_t buf_size,
    int percent,
    int width
) {

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int filled = (percent * width) / 100;

    char *p = buf;
    if (buf_size < 5) {
        buf[0] = '\0';
        return;
    }
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