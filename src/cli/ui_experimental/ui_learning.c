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
    const char *header;
    //const char *time;
    const char *controls;
    const BoxBorders *borders;
    Timer *timer;
} TimerScreenState;




static char* append_repeat(char *buf, const char *ch, int count) {
    for (int i = 0; i < count; i++) {
        strcpy(buf, ch);
        buf += strlen(ch); 
    }
    return buf;
}

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

void box_format_line(char *buf, const char *str, const Border *border, int width) {
    char *p = buf;
    int slen = strlen(str);

    int total_pad = width - slen;
    int left_pad  = total_pad / 2;
    int right_pad = total_pad - left_pad;

    p = append_repeat(p, border->left_char, 1);
    p = append_repeat(p, border->mid_char, left_pad);

    memcpy(p, str, slen);
    p += slen;

    p = append_repeat(p, border->mid_char, right_pad);
    p = append_repeat(p, border->right_char, 1);

    *p = '\0';
}

void box_render_line(
    char *buf,
    const char *str,
    const Border *border,
    int width
) {
    box_format_line(buf, str, border, width);
    printf("%s\n", buf);
}

void timer_screen_render(TimerScreenState *screenState) {
    char buf[128];
    char time_buf[16];
    char category_buf[64];    
    int w = screenState->width;

    snprintf(
        category_buf,
        sizeof category_buf,
        "%s -> %s",
        screenState->timer->category,
        screenState->timer->subcategory
    );

    TimeDisplay timeDisplay = get_time_display(screenState->timer);
    snprintf(
        time_buf,
        sizeof time_buf,
        "%02d:%02d:%02d",
        timeDisplay.minutes,
        timeDisplay.seconds,
        timeDisplay.milliseconds / 10
    );


    char bar[128];

    int percent = calculate_progress(screenState->timer);
    progress_bar_to_buf(bar, sizeof bar, percent, w / 2);


    // Header
    box_render_line(buf, "", screenState->borders->top, w);
    box_render_line(buf, screenState->header, screenState->borders->mid, w);
    box_render_line(buf, "", screenState->borders->mid_bottom, w);

    // Time
    box_render_line(buf, "", screenState->borders->mid, w);
    box_render_line(buf, time_buf, screenState->borders->mid, w);
    box_render_line(buf, bar, screenState->borders->mid, w * 2);
    box_render_line(buf, "", screenState->borders->mid, w);

    // Category
    box_render_line(buf, category_buf, screenState->borders->mid, w);
    box_render_line(buf, "", screenState->borders->mid, w);

    //Controls
    box_render_line(buf, screenState->controls, screenState->borders->mid, w);
    box_render_line(buf, "", screenState->borders->mid, w);
    box_render_line(buf, "", screenState->borders->bottom, w);
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
        .header = "POMODORO TIMER",
        .controls = "[Space] Pause  [Q] Quit",
        .borders = &borders,
        .timer = &timer
    };
    timer_screen_render(&content);
    return 0;
}
