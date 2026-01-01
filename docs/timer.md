# Documentation for the timer module

## Dependencies

- `#define _GNU_SOURCE` - enables GNU extensions and exposes newer POSIX APIs on glibc systems, including `clock_gettime()`
## Logic

### Time Tracking Model 

The timer uses a **start-accumulate-pause-resume** pattern: 
- `started_at_ms` records when the timer last started/resumed 
- `accumulated_ms` stores total elapsed time from previous run segments 
- Elapsed time:
  ```
  Current elapsed time =
    accumulated_ms +
    (now - started_at_ms)   if timer_state == STATE_ACTIVE
  ```

### The Workflow 

**On Create:**
1. Call `Timer* create_timer(int minutes, TimerMode timer_mode, TimerWorkMode timer_mode_work, const char *category, const char *activity)`
2. Allocates memory for the timer and initializes its fields, including copying the `category` and `activity` strings. The initial state is set to `STATE_CREATED`, with `started_at_ms = 0` (the timer is treated as not running)
3. Returns the pointer to created timer

**On Start / Resume:** 
1. Call `void start_timer(Timer *timer)`
2. Record current timestamp in `started_at_ms`: 
   `timer->started_at_ms = get_current_ms();`
3. Transition to `STATE_ACTIVE`

**On Pause:** 
1. Call `void pause_timer(Timer *timer)`
2. Add elapsed segment: 
   `timer->accumulated_ms += (get_current_ms() - timer->started_at_ms)` 
3. Transition to `STATE_PAUSED` 

**On Cancel:**
1. Call `void cancel_timer(Timer *timer)`
2. Update `timer->accumulated_ms` if `timer->timer_state == STATE_ACTIVE`
3. Set `timer->started_at_ms = 0` - timer is treated as not running
4. Transition to `STATE_CANCELLED`

**Is Finished:**
1. Call `bool is_finished_timer(Timer *timer)`
2. (Probably FIX) `MODE_STOPWATCH` => `false`
3. `MODE_COUNTDOWN` =>
	- Calculate elapsed time: 
	  `elapsed = accumulated + (current - started_at) * (bool)(is_active)`
	- `elapsed >= target` => transition to `STATE_COMPLETED`, `return true`
	- `elapsed < target` => `return false` 

**Display Time:** 
1. Call `TimerDisplay get_time_display(const Timer *timer)`
2. Calculate elapsed time:
   `elapsed = accumulated + (current - started_at) * (bool)(is_active)`
3. Calculate `display_ms`:
	- `MODE_STOPWATCH` => `display_ms = elapsed_ms`
	- `MODE_COUNTDOWN` => `display_ms = max(0, timer->target_ms - elapsed_ms)`
4. Calculate values to display (minutes, seconds, centiseconds)

### Notes
- Elapsed time is calculated using the helper function
  `int64_t get_elapsed_ms(const Timer *timer)`