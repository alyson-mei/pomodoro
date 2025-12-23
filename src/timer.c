#define _GNU_SOURCE

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>

#include "../include/timer.h"


int64_t get_current_ms(void) {
    struct timespec tspec;
    clock_gettime(CLOCK_MONOTONIC, &tspec);

    return tspec.tv_sec * 1000 + tspec.tv_nsec / 1000000;
}

int64_t get_elapsed_ms(const Timer *timer) {
    int64_t elapsed = timer->accumulated_ms;

    if (timer->timer_state == STATE_ACTIVE && timer->started_at_ms > 0) {
        elapsed += get_current_ms() - timer->started_at_ms;
    }

    return elapsed;
}

Timer* create_timer(
    int minutes,
    TimerMode timer_mode, 
    TimerWorkMode timer_mode_work,
    const char *category,
    const char *subcategory
) {
    Timer *timer = malloc(sizeof(Timer)); 
    if (!timer) return NULL;

    *timer = (Timer){
        .started_at_ms  = 0,
        .target_ms      = minutes * 60 * 1000,
        .accumulated_ms = 0,
        .timer_mode     = timer_mode,
        .timer_work_mode = timer_mode_work,
        .timer_state    = STATE_ACTIVE,
    };    
    
    strncpy(timer->category, category, sizeof timer->category - 1);
    timer->category[sizeof timer->category - 1] = '\0';
    strncpy(timer->subcategory, subcategory, sizeof timer->subcategory - 1);
    timer->subcategory[sizeof timer->subcategory - 1] = '\0';

    return timer;
}

void start_timer(Timer *timer) {
    timer->started_at_ms = get_current_ms();
    timer->timer_state = STATE_ACTIVE;    
}

void pause_timer(Timer *timer) {
    timer->accumulated_ms += (get_current_ms() - timer->started_at_ms);
    timer->timer_state = STATE_PAUSED;
}

void cancel_timer(Timer *timer) {
    if (timer->timer_state == STATE_ACTIVE) {
        timer->accumulated_ms += get_current_ms() - timer->started_at_ms;
    }
    timer->started_at_ms = 0;
    timer->timer_state = STATE_CANCELLED;
}

bool is_finished_timer(Timer *timer) {
    if (timer->timer_mode != MODE_COUNTDOWN) return false;

    int64_t now = get_current_ms();
    int64_t elapsed = timer->accumulated_ms;

    if (timer->timer_state == STATE_ACTIVE)
        elapsed += now - timer->started_at_ms;
    
    if (elapsed >= timer->target_ms) {
        timer->timer_state = STATE_COMPLETED;
        return true;
    }
    else 
        return false;
}

DisplayTime get_time_display(const Timer *timer) {
    int64_t now_ms = get_current_ms();
    int64_t elapsed_ms = timer->accumulated_ms;
    if (timer->timer_state == STATE_ACTIVE) {
        elapsed_ms += now_ms - timer->started_at_ms;
    }

    int64_t display_ms;
    if (timer->timer_mode == MODE_STOPWATCH) {
        display_ms = elapsed_ms;
    } else {
        display_ms = timer->target_ms - elapsed_ms;
    }

    DisplayTime td;
    int64_t ms = display_ms < 0 ? 0 : display_ms;

    td.centiseconds = (ms / 10) % 100;
    td.seconds = (ms / 1000) % 60;
    td.minutes = ms / (1000 * 60);

    return td;
}
