#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Timer Timer;

typedef struct {
    int minutes;
    int seconds;
    int milliseconds; //TODO: consider changing to centiseconds
} TimeDisplay;

typedef enum {
    MODE_COUNTDOWN,
    MODE_STOPWATCH
} TimerMode;

TimeDisplay get_time_display(const Timer *timer);
Timer* ptimer_create(int minutes, TimerMode mode);
void ptimer_start(Timer *t);
void ptimer_pause(Timer *t);
void ptimer_resume(Timer *t);
bool ptimer_finished(const Timer *t);

#endif
