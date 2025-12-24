#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#include "../include/timer.h"
#include "../include/display.h"
#include "../include/global.h"

// Setup non-blocking input
void setup_terminal(struct termios *old_tio) {
    struct termios new_tio;
    
    tcgetattr(STDIN_FILENO, old_tio);  // Save old settings
    new_tio = *old_tio;
    
    new_tio.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // Make stdin non-blocking
}

void restore_terminal(struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    char category[] = "Coding";
    char subcategory[] = "Building a Pomodoro App";

    Timer* timer = create_timer(
        5,
        MODE_COUNTDOWN,
        MODE_WORK,
        category,
        subcategory
    );

    struct termios old_tio;
    setup_terminal(&old_tio);

    start_timer(timer);

    while (timer->timer_state != STATE_COMPLETED && timer->timer_state != STATE_CANCELLED) {
        // Check for keyboard input
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == ' ') {
                if (timer->timer_state == STATE_PAUSED) {
                    start_timer(timer);
                } else if (timer->timer_state == STATE_ACTIVE) {
                    pause_timer(timer);
                }
            } else if (c == 'q' || c == 'Q') {
                cancel_timer(timer);
                break;
            }
        }
        
        // Check if timer finished
        is_finished_timer(timer);
        
        pomodoro_render(timer, 1, 4);
        usleep(50000);
    }

    
    pomodoro_render(timer, 1, 4);
    
    restore_terminal(&old_tio);
    printf("\033[?25h"); // restore cursor
    
    return 0;
}