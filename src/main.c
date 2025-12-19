#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>

#include "../include/timer.h"
#include "../include/display.h"

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    char category[] = "category";
    char subcategory[] = "subcategory";

    Timer* t = ptimer_create(
        1,
        MODE_COUNTDOWN,
        category,
        subcategory
    );

    ptimer_start(t);

    while (!ptimer_finished(t)) {
        pomodoro_render(t);
        usleep(50000);
    }
    pomodoro_render(t);
    printf("\033[?25h"); // restore cursor
    return 0;
}
