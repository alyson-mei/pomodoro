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
    size_t header_size,
    TimerMode mode, 
    TimerWorkMode work_mode,
    TimerState state,
    int current_iteration,
    int total_iterations
) {
    const char *mode_str = (mode == MODE_COUNTDOWN) ? TIMER_COUNTDOWN : TIMER_STOPWATCH;
    const char *work_str;
    
    if (state == STATE_CANCELLED) {
        work_str = TIMER_CANCELLED;
    } else {
        switch (work_mode) {
            case MODE_WORK:       work_str = TIMER_WORK; break;
            case MODE_BREAK:      work_str = TIMER_BREAK; break;
            case MODE_LONG_BREAK: work_str = TIMER_LONG_BREAK; break;
            default:              work_str = mode_str; // Fallback
                                  snprintf(header, header_size, "%s", mode_str);
                                  return;
        }
    }
    
    // Single formatting logic
    if (mode == MODE_COUNTDOWN && total_iterations >= 2) {
        snprintf(header, header_size, "%s: %s (%d / %d)", 
                 mode_str, work_str, current_iteration, total_iterations);
    } else {
        snprintf(header, header_size, "%s: %s", mode_str, work_str);
    }
}

const char* get_controls_text(TimerState state) {
    switch (state) {
        case STATE_ACTIVE:    return CONTROLS_ACTIVE;
        case STATE_PAUSED:    return CONTROLS_PAUSED;
        case STATE_COMPLETED: return CONTROLS_COMPLETED;
        case STATE_CANCELLED: return CONTROLS_CANCELLED;
        default:              return " ";
    }
}

const char* get_state_color(const Colors *colors, TimerState state) {
    switch (state) {
        case STATE_ACTIVE:    return colors->active;
        case STATE_PAUSED:    return colors->paused;
        case STATE_COMPLETED: return colors->completed;
        case STATE_CANCELLED: return colors->cancelled;
        default:              return colors->active;
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
    int width,
    const ProgressBar *pb 
) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    const size_t min_size = 5 + 6 + 1;
    if (buf_size < min_size) {
        buf[0] = '\0';
        return;
    }

    int filled = (percent * width) / 100;
    memset(buf, ' ', 5);
    char *p = buf + 5;
    size_t remaining = buf_size - 5 - 6;

    // Cache character lengths
    size_t left_len = strlen(pb->left_char);
    size_t on_len = strlen(pb->mid_char_on);
    size_t off_len = strlen(pb->mid_char_off);
    size_t right_len = strlen(pb->right_char);

    // Left bracket
    if (left_len > 0 && left_len <= remaining) {
        memcpy(p, pb->left_char, left_len);
        p += left_len;
        remaining -= left_len;
    }

    // Progress bar
    for (int i = 0; i < width && remaining > 0; ++i) {
        const char *ch;
        size_t ch_len;
        
        if (i < filled) {
            ch = pb->mid_char_on;
            ch_len = on_len;
        } else {
            ch = pb->mid_char_off;
            ch_len = off_len;
        }
        
        if (ch_len > remaining) break;
        memcpy(p, ch, ch_len);
        p += ch_len;
        remaining -= ch_len;
    }

    // Right bracket
    if (right_len > 0 && right_len <= remaining) {
        memcpy(p, pb->right_char, right_len);
        p += right_len;
        remaining -= right_len;
    }

    // Percentage
    snprintf(p, buf_size - (p - buf), " %3d%%", percent);
}
