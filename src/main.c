#define _GNU_SOURCE
#include <unistd.h>
#include "timer.h"
#include "display.h"

int main() {
    Timer* t = ptimer_create(1, MODE_COUNTDOWN);
    
    ptimer_start(t);
    do  {
        display_render(t);
        usleep(10000);
    }
    while (!ptimer_finished(t));

    return 0;
}