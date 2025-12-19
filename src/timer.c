#define _GNU_SOURCE

#include <time.h>
#include <stdlib.h>
#include<string.h>

#include <stdint.h>
#include <stdbool.h>

#include "../include/timer.h"

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

Timer* ptimer_create(
    int minutes,
    TimerMode mode, 
    const char *category,
    const char *subcategory
) {
    Timer *t = malloc(sizeof(Timer)); 
    if (!t) return NULL;

    t->started_at_ms = 0;
    t->target_ms = minutes * 60 * 1000;
    t->accumulated_ms = 0;
    t->mode = mode;
    t->is_paused = false;
    
    strncpy(t->category, category, sizeof t->category - 1);
    t->category[sizeof t->category - 1] = '\0';
    strncpy(t->subcategory, subcategory, sizeof t->subcategory - 1);
    t->subcategory[sizeof t->subcategory - 1] = '\0';

    return t;
}

int64_t ptimer_elapsed_ms(const Timer *t) {
    int64_t elapsed = t->accumulated_ms;

    if (!t->is_paused && t->started_at_ms > 0) {
        elapsed += get_current_ms() - t->started_at_ms;
    }

    return elapsed;
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