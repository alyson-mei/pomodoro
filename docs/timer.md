# A documentation on timer module

## Logic

### Time Tracking Model 

The timer uses a **start-accumulate-pause-resume** pattern: 
- `started_at_ms` records when the timer last started/resumed 
- `accumulated_ms` stores total elapsed time from previous run segments 
- Current elapsed time = `accumulated_ms + (now - started_at_ms)` (when active) 

### Calculation Flow 

**On Start:** 
- Record current timestamp in `started_at_ms` 
- `accumulated_ms` remains 0 

**On Pause:** 
- Add elapsed segment: `accumulated_ms += (now - started_at_ms)` 
- Transition to `STATE_PAUSED` 

**On Resume:** 
- Update `started_at_ms` to current time (starts new segment) 
- `accumulated_ms` preserves previous elapsed time 
- Transition to `STATE_ACTIVE` 

**Display Time:** 
- **Stopwatch**: shows `accumulated_ms + (now - started_at_ms)` 
- **Countdown**: shows `target_ms - elapsed_ms` (remaining time) 

## Dependencies

- `#define _GNU_SOURCE` - this macro enables GNU extensions and newer [POSIX](https://en.wikipedia.org/wiki/POSIX) features. A dependency for `clock_gettime()`.
- Everything else is standard.

## Structures

### `enum TimerMode`

- Countdown or stopwatch mode.
### `enum TimerState` 

- Active, paused, completed, or cancelled.
### `struct Timer`

Represents a single timer instance.
- `started_at_ms` — timestamp (ms) of the last start or resume.
- `target_ms` — timestamp (ms) when the countdown completes (unused in stopwatch mode).
- `accumulated_ms` — total elapsed time since the first start.
- `timer_mode`, `timer_state`.
- `category`, `subcategory` — labels for history tracking.

### `struct TimeDisplay`

- Holds the time values in minutes, seconds and centiseconds to display.

## Functions

### `int64_t get_current_ms(void)`

Returns the current time in milliseconds using a monotonic clock.

Relies on:
- `struct timespec` — holds seconds (`tv_sec`) and nanoseconds (`tv_nsec`).
- `clock_gettime` — POSIX function for retrieving high-resolution time; uses `CLOCK_MONOTONIC` to avoid issues with system clock adjustments.

### `Timer *create_timer(int minutes, TimerMode mode, const char *category, const char *subcategory)`

Allocates and initializes a new `Timer` instance.

- Allocates memory for the timer and returns `NULL` on failure.    
- Initializes time fields to zero.
- Sets the timer mode and target duration (`minutes` → milliseconds).
- Copies `category` and `subcategory` strings with guaranteed null-termination.

Returns a pointer to the created `Timer`, or `NULL` if allocation fails.

### Controls:

- `start_timer(Timer *timer)` - starts / resumes timer. Sets 
  `timer->started_at_ms` to current time, `timer->timer_state` to `STATE_ACTIVE`
- `pause_timer(Timer *timer)` - pauses timer. Updates `timer->accumulated_ms`, sets `timer->timer_state` to `STATE_PAUSED`.
- `cancel_timer(Timer *timer)` - sets `timer->timer_state` to `STATE_CANCELLED`.

### `bool is_finished_timer(Timer *timer)`

Checks if the time countdown has completed, sets `timer->timer_state` to `STATE_COMPLETED` when complete.

### `DisplayTime get_time_display(const Timer *timer)`

Calculates and returns the time to display on the screen. 
