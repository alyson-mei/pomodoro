#include <unistd.h>
#include <stdio.h>
#include "timer.h"

void display_render(const Timer *t) {
    TimeDisplay td = get_time_display(t);
    printf("\r\033[K");
    printf("%02d:%02d:%02d", td.minutes, td.seconds, td.milliseconds / 10);
    fflush(stdout);
}