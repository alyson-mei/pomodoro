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
} Borders;

typedef struct 
{
    int width;
    const char *header;
    const char *time;
    const char *controls;
    const Borders *borders;
   // Timer *timer;
} TimerScreenState;

static char* repeat_char(char *buf, const char *ch, int count) {
    for (int i = 0; i < count; i++) {
        strcpy(buf, ch);
        buf += strlen(ch); 
    }
    return buf;
}

// void draw_progress_bar(int percent, int width) {
//     if (percent < 0) percent = 0;
//     if (percent > 100) percent = 100;

//     int filled = (percent * width) / 100;

//     for (int i = 0; i < width; ++i) {
//         if (i < filled)
//             printf("█");
//         else
//             printf("░");
//     }
// }

// int calculate_progress(const Timer *t) {
//     if (t->target_ms <= 0) return 100;

//     int64_t elapsed = ptimer_elapsed_ms(t);

//     if (elapsed <= 0) return 0;
//     if (elapsed >= t->target_ms) return 100;

//     return (int)((elapsed * 100) / t->target_ms);
// }

void box_string(char *buf, const char *str, const Border *border, int width) {
    char *p = buf;
    int slen = strlen(str);

    int total_pad = width - slen;
    int left_pad  = total_pad / 2;
    int right_pad = total_pad - left_pad;

    p = repeat_char(p, border->left_char, 1);
    p = repeat_char(p, border->mid_char, left_pad);

    memcpy(p, str, slen);
    p += slen;

    p = repeat_char(p, border->mid_char, right_pad);
    p = repeat_char(p, border->right_char, 1);

    *p = '\0';
}


void content_render(
    char *buf,
    const char *str,
    const Border *border,
    int width
) {
    box_string(buf, str, border, width);
    printf("%s\n", buf);
}

void box_render(TimerScreenState *screenState) {
    char buf[128];
    int w = screenState->width;

    content_render(buf, "", screenState->borders->top, w);
    content_render(buf, screenState->header, screenState->borders->mid, w);
    content_render(buf, "", screenState->borders->mid_bottom, w);

    content_render(buf, "", screenState->borders->mid, w);
    content_render(buf, screenState->time, screenState->borders->mid, w);
    content_render(buf, "", screenState->borders->mid, w);

    content_render(buf, screenState->controls, screenState->borders->mid, w);
    content_render(buf, "", screenState->borders->mid, w);
    content_render(buf, "", screenState->borders->bottom, w);
}


int main() {
    char buf[128];

    static const Border top_border     = {"╔", "═", "╗"};
    static const Border mid_border     = {"║", " ", "║"};
    static const Border midb_border    = {"╠", "═", "╣"};
    static const Border bottom_border  = {"╚", "═", "╝"};

    static const Borders borders = {
        .top        = &top_border,
        .mid        = &mid_border,
        .mid_bottom = &midb_border,
        .bottom     = &bottom_border
    };

    Timer timer = {
        .accumulated_ms = 5000,
        .started_at_ms = 2000,
        .target_ms = 7000,
        .mode = "countdown",
        .is_paused = false,
        .category = "cat",
        .subcategory = "subcat"
    };

    TimerScreenState content = {
        .width = 40,
        .header = "POMODORO TIMER",
        .time = "10:20:30",
        .controls = "[Space] Pause  [Q] Quit",
        .borders = &borders
    };
    box_render(&content);
    return 0;
}
