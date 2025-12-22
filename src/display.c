#include <unistd.h>
#include <stdio.h>
#include "../include/timer.h"

void display_render(const Timer *t) {
    DisplayTime td = get_time_display(t);
    printf("\r\033[K");
    printf("%02d:%02d:%02d", td.minutes, td.seconds, td.centiseconds);
    fflush(stdout);
}