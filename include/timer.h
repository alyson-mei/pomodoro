#ifndef TIMER_H
#define TIMER_H

#define BUF_SIZE 256 //TODO: replace to common header

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MODE_COUNTDOWN,
    MODE_STOPWATCH
} TimerMode;

typedef struct {
    int64_t started_at_ms;
    int64_t target_ms;
    int64_t accumulated_ms;
    TimerMode mode;
    bool is_paused;
    bool is_canceled;
    char category[BUF_SIZE];
    char subcategory[BUF_SIZE];
} Timer;

typedef struct {
    int minutes;
    int seconds;
    int milliseconds; //TODO: consider changing to centiseconds
} TimeDisplay;

int64_t get_current_ms(void);
int64_t ptimer_elapsed_ms(const Timer *t);

TimeDisplay get_time_display(const Timer *timer);
Timer* ptimer_create(int minutes, TimerMode mode, const char *category, const char *subcategory);
void ptimer_start(Timer *t);
void ptimer_pause(Timer *t);
void ptimer_resume(Timer *t);
bool ptimer_finished(const Timer *t);

#endif
