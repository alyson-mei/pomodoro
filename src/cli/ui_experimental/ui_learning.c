#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>

#include "../../../include/timer.h"
#include "../../../include/display.h"

typedef struct {
    const char *left_char;
    const char *mid_char;
    const char *right_char;
} Border;

typedef struct {
    const Border *top;
    const Border *mid;
    const Border *mid_bottom;
    const Border *bottom;
} BoxBorders;

typedef struct 
{
    int width;
    const char *controls;
    const BoxBorders *borders;
    Timer *timer;
} TimerScreenState;

typedef struct
{
    char header[64];
    char time_str[16];
    char progress_bar[128];
    char category_str[64];
} TimerScreenView;

// Helper functions

int calculate_progress(const Timer *t) {
    if (!t || t->target_ms <= 0)
        return 100;

    int64_t elapsed = ptimer_elapsed_ms(t);

    if (elapsed <= 0)
        return 0;
    if (elapsed >= t->target_ms)
        return 100;

    return (int)((elapsed * 100) / t->target_ms);
}

static char* repeat_string(char *buf, const char *ch, int count) {
    for (int i = 0; i < count; i++) {
        strcpy(buf, ch);
        buf += strlen(ch); 
    }
    return buf;
}

// Writing values to buffer

void progress_bar_to_buf(
    char *buf,
    size_t buf_size,
    int percent,
    int width
) {

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int filled = (percent * width) / 100;

    char *p = buf;
    memset(p, ' ', 5);
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

    char buf_percent[16];
    snprintf(buf_percent, sizeof buf_percent, " %3d%%", percent);
    strcat(p, buf_percent);
    
}


void box_line_to_buf(char *buf, const char *str, const Border *border, int width) {
    char *p = buf;
    int slen = strlen(str);

    int total_pad = width - slen;
    int left_pad  = total_pad / 2;
    int right_pad = total_pad - left_pad;

    p = repeat_string(p, border->left_char, 1);
    p = repeat_string(p, border->mid_char, left_pad);

    memcpy(p, str, slen);
    p += slen;

    p = repeat_string(p, border->mid_char, right_pad);
    p = repeat_string(p, border->right_char, 1);

    *p = '\0';
}

// Building a view

void timer_screen_build_view(TimerScreenState *state, TimerScreenView *view) {
    if (state->timer->mode == MODE_COUNTDOWN) {
        strcpy(view->header, "POMODORO TIMER");
    }
    else {
        strcpy(view->header, "STOPWATCH TIMER");
    }
    
    TimeDisplay td = get_time_display(state->timer);
    snprintf(view->time_str, sizeof view->time_str,
             "%02d:%02d:%02d", td.minutes, td.seconds, td.milliseconds / 10);

    int percent = calculate_progress(state->timer);
    progress_bar_to_buf(view->progress_bar, sizeof view->progress_bar,
                            percent, state->width / 2);

    snprintf(view->category_str, sizeof view->category_str,
            "%s -> %s", state->timer->category, state->timer->subcategory);

}

// Rendering 

void box_render_line(
    char *buf,
    const char *str,
    const Border *border,
    int width
) {
    box_line_to_buf(buf, str, border, width);
    printf("%s\n", buf);
}

void timer_screen_render(TimerScreenState *state, TimerScreenView *view) {
    char buf[256];
    int w = state->width;

    // Header
    box_render_line(buf, "", state->borders->top, w);
    box_render_line(buf, view->header, state->borders->mid, w);
    box_render_line(buf, "", state->borders->mid_bottom, w);

    // Time
    box_render_line(buf, "", state->borders->mid, w);
    box_render_line(buf, view->time_str, state->borders->mid, w);
    box_render_line(buf, view->progress_bar, state->borders->mid, w * 2);
    box_render_line(buf, "", state->borders->mid, w);

    // Category
    box_render_line(buf, view->category_str, state->borders->mid, w);
    box_render_line(buf, "", state->borders->mid, w);

    // Controls
    box_render_line(buf, state->controls, state->borders->mid, w);
    box_render_line(buf, "", state->borders->mid, w);
    box_render_line(buf, "", state->borders->bottom, w);
}

int main() {
    char buf[128];

    static const Border top_border     = {"╔", "═", "╗"};
    static const Border mid_border     = {"║", " ", "║"};
    static const Border midb_border    = {"╠", "═", "╣"};
    static const Border bottom_border  = {"╚", "═", "╝"};

    static const BoxBorders borders = {
        .top        = &top_border,
        .mid        = &mid_border,
        .mid_bottom = &midb_border,
        .bottom     = &bottom_border
    };

    Timer timer = {
        .accumulated_ms = 5000,
        .started_at_ms = get_current_ms(),
        .target_ms = 15000,
        .mode = MODE_COUNTDOWN,
        .is_paused = false,
        .category = "cat",
        .subcategory = "subcat"
    };

    TimerScreenState content = {
        .width = 40,
        .controls = "[Space] Pause  [Q] Quit",
        .borders = &borders,
        .timer = &timer
    };

    TimerScreenView view;

    timer_screen_build_view(&content, &view);
    timer_screen_render(&content, &view);
    return 0;
}
