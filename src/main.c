#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#include "../include/timer.h"
#include "../include/ui.h"
#include "../include/config.h"
#include "../include/literals.h"
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

    Settings *settings = load_config("config.ini");
    if (!settings) {
        printf("Failed to load config\n");
        return 1;
    }
    
    struct termios old_tio;
    setup_terminal(&old_tio);

    char c;
    
    int time;
    int cycle;
    TimerWorkMode work_mode;
    

    for (int j = 0; j < settings->countdown.num_cycles * 2 + 1; j++) {
        if (j < settings->countdown.num_cycles * 2) {
            if (j % 2 == 0) {
                work_mode = MODE_WORK;
                time = settings->countdown.work_minutes;
                cycle = j / 2 + 1;
            }
            else {
                work_mode = MODE_BREAK;
                time = settings->countdown.break_minutes;
                cycle = j / 2 + 1;
            }
        }
        else {
            work_mode = MODE_LONG_BREAK;
            time = settings->countdown.long_break_minutes;
            cycle = j / 2;
        }

        Timer* timer = create_timer(
            time,
            MODE_COUNTDOWN,
            work_mode,
            settings->activity.category,
            settings->activity.activity
        );

        start_timer(timer);

        while (timer->timer_state != STATE_COMPLETED && timer->timer_state != STATE_CANCELLED) {
            // Check for keyboard input
            if (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == ' ') {
                    if (timer->timer_state == STATE_PAUSED) {
                        start_timer(timer);
                    } else if (timer->timer_state == STATE_ACTIVE) {
                        pause_timer(timer);
                    }
                } else if (c == 'q' || c == 'Q') {
                    cancel_timer(timer);
                    render_ui(
                        timer, 
                        settings->ui.color_theme,
                        settings->ui.border_type,
                        cycle, 
                        settings->countdown.num_cycles
                    );
                    free(timer);
                    goto cleanup;
                }
            }
            
            is_finished_timer(timer);
            
            render_ui(
                timer, 
                settings->ui.color_theme,
                settings->ui.border_type,
                cycle, 
                settings->countdown.num_cycles
            );
            usleep(50000);
        }

        render_ui(
            timer, 
            settings->ui.color_theme,
            settings->ui.border_type,
            cycle, 
            settings->countdown.num_cycles
        );

        // Play sound and wait for input (if not last phase)
        if (j < settings->countdown.num_cycles * 2) {  // Changed this line
            printf("\a");  // Bell sound
            fflush(stdout);
            
            // Switch to blocking mode for waiting
            int flags = fcntl(STDIN_FILENO, F_GETFL);
            fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
            
            // Wait for Enter or Q
            while (1) {
                if (read(STDIN_FILENO, &c, 1) == 1) {
                    if (c == '\n' || c == '\r') {
                        break;  // Continue to next cycle
                    } else if (c == 'q' || c == 'Q') {
                        free(timer);
                        goto cleanup;
                    }
                }
            }
            
            // Switch back to non-blocking
            fcntl(STDIN_FILENO, F_SETFL, flags);
        }
        
        free(timer);
    }

cleanup:
    restore_terminal(&old_tio);
    printf("\033[?25h"); // restore cursor
    free_config(settings);
    
    return 0;
}