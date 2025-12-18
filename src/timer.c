#define _GNU_SOURCE
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/timer.h"

struct Timer {
    int64_t started_at_ms;
    int64_t target_ms;
    int64_t accumulated_ms;
    TimerMode mode;
    bool is_paused;
};

int64_t get_current_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

TimeDisplay get_time_display(const Timer *timer) {
    int64_t now_ms = get_current_ms();
    int64_t elapsed_ms = timer->accumulated_ms;
    if (!timer->is_paused) {
        elapsed_ms += now_ms - timer->started_at_ms;
    }

    int64_t display_ms;
    if (timer->mode == MODE_STOPWATCH) {
        display_ms = elapsed_ms;
    } else {
        display_ms = timer->target_ms - elapsed_ms;
    }

    TimeDisplay td;
    int64_t ms = display_ms < 0 ? 0 : display_ms;

    td.milliseconds = ms % 1000;
    td.seconds = (ms / 1000) % 60;
    td.minutes = ms / (1000 * 60);

    return td;
}

Timer* ptimer_create(int minutes, TimerMode mode) {
    Timer *t = malloc(sizeof(Timer)); 
    t->started_at_ms = 0;
    t->target_ms = minutes * 60 * 1000;
    t->accumulated_ms = 0;
    t->mode = mode;
    t->is_paused = false;
    return t;
}

void ptimer_start(Timer *t) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    t->started_at_ms = get_current_ms();    
}

void ptimer_pause(Timer *t) {
    t->accumulated_ms += (get_current_ms() - t->started_at_ms);
    t->is_paused = true;
}

void ptimer_resume(Timer *t) {
    t->started_at_ms = get_current_ms();
    t->is_paused = false;
}

bool ptimer_finished(const Timer *t) {
    if (t->mode != MODE_COUNTDOWN) return false;

    int64_t now = get_current_ms();
    int64_t elapsed = t->accumulated_ms;

    if (!t->is_paused)
        elapsed += now - t->started_at_ms;

    return elapsed >= t->target_ms;
}