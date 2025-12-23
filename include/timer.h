#ifndef TIMER_H
#define TIMER_H

#define BUF_SIZE 256 //TODO: replace to common header

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MODE_COUNTDOWN,
    MODE_STOPWATCH
} TimerMode;

typedef enum {
    MODE_WORK,
    MODE_BREAK,
    MODE_LONG_BREAK
} TimerWorkMode;

typedef enum {
    STATE_ACTIVE,
    STATE_PAUSED,
    STATE_COMPLETED,
    STATE_CANCELLED
} TimerState;

typedef struct {
    int64_t started_at_ms;
    int64_t target_ms;
    int64_t accumulated_ms;
    TimerMode timer_mode;
    TimerWorkMode timer_work_mode;
    TimerState timer_state;
    char category[BUF_SIZE];
    char subcategory[BUF_SIZE];
} Timer;

typedef struct {
    int minutes;
    int seconds;
    int centiseconds;
} DisplayTime;

int64_t get_current_ms(void);
int64_t get_elapsed_ms(const Timer *timer);

DisplayTime get_time_display(const Timer *timer);
Timer* create_timer(
    int minutes,
    TimerMode timer_mode, 
    TimerWorkMode timer_mode_work,
    const char *category,
    const char *subcategory
);
void start_timer(Timer *timer);
void pause_timer(Timer *timer);
void cancel_timer(Timer *timer);
bool is_finished_timer(Timer *timer);

#endif
