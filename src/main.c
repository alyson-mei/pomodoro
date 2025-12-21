#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#include "../include/timer.h"
#include "../include/display.h"

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

    Timer* t = ptimer_create(
        1,
        MODE_COUNTDOWN,
        category,
        subcategory
    );

    struct termios old_tio;
    setup_terminal(&old_tio);

    ptimer_start(t);

    while (!ptimer_finished(t)) {
        // Check for keyboard input
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == ' ') {
                t->is_paused = !t->is_paused;  // Toggle pause
                if (t->is_paused) {
                    ptimer_pause(t);
                } else {
                    ptimer_resume(t);
                }
            } else if (c == 'q' || c == 'Q') {
                t->is_canceled = true;
                break;  // Quit
            }
        }
        
        pomodoro_render(t);
        usleep(50000);
    }
    
    pomodoro_render(t);
    
    restore_terminal(&old_tio);
    printf("\033[?25h"); // restore cursor
    
    return 0;
}