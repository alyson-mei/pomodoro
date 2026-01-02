#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "../include/global.h"
#include "../include/timer.h"
#include "../include/config.h"
#include "../include/literals.h"
#include "../include/data.h"

#include "../include/ui/ui.h"
#include "../include/ui/command.h"
#include "../include/ui/setup.h"

#define TEMP_FILE "data/.temp_entry"


// TODO:
// - Short term:
//      - Add a message after a session
//      - Sessions logic (outer loop)
//      - Improve the STOPWATCH mode desging
// - Long term:
//      - Performance improvement (hoisting)
//      - Category and activity should be on different lines
//      - Edit entries
// Thoughts: 
//      - Version for the database; maybe also some additional fields
//          - e.g. some id for the session? Should think about it
//      - Comments - add an option to left a message after each cycle? Or after each session?

// Ensure data directory exists


int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    
    ensure_data_dir();
    check_crashed_session();

    Settings *settings = load_config("./config.ini");
    if (!settings) {
        printf("Failed to load config\n");
        return 1;
    }
    
    struct termios old_tio;
    setup_terminal(&old_tio);

    char c;
    int duration;  // Instead of 'time'
    int cycle;
    TimerWorkMode work_mode;

    // Track time for periodic saves (every 60 seconds)
    time_t last_save = 0;

    for (int j = 0; j < settings->countdown.num_cycles * 2; j++) {
        cycle = j / 2 + 1;

        if (j < settings->countdown.num_cycles * 2 - 1) {
            if (j % 2 == 0) {
                work_mode = MODE_WORK;
                duration = settings->countdown.work_minutes;
            }
            else {
                work_mode = MODE_BREAK;
                duration = settings->countdown.break_minutes;
            }
        }
        else {
            work_mode = MODE_LONG_BREAK;
            duration = settings->countdown.long_break_minutes;
        }

        Timer* timer = create_timer(
            duration,
            MODE_STOPWATCH,
            work_mode,
            settings->activity.category,
            settings->activity.activity
        );


        // Create history entry for this timer
        HistoryEntry entry = create_history_entry(timer, NULL);
        last_save = time(NULL);

        start_timer(timer);

        while (timer->timer_state != STATE_COMPLETED && 
               timer->timer_state != STATE_CANCELLED) {
            
            // Check for keyboard input
            Command cmd = read_command();

            switch (cmd) {
                case CMD_TOGGLE_PAUSE:
                    if (timer->timer_state == STATE_PAUSED) {
                        start_timer(timer);
                    } else if (timer->timer_state == STATE_ACTIVE) {
                        pause_timer(timer);
                    }
                    break;

                case CMD_QUIT:
                    cancel_timer(timer);
                    
                    // Update entry before saving
                    set_entry_elapsed_completed(&entry, timer);
                    
                    // Only save temp if elapsed >= 60 seconds
                    if (entry.elapsed_seconds >= 60) {
                        save_temp_entry(&entry);
                    }
                    
                    render_ui(
                        timer, 
                        settings->ui.color_theme,
                        settings->ui.border_type,
                        cycle, 
                        settings->countdown.num_cycles
                    );
                    
                    free(timer);
                    
                    // Free entry strings
                    free(entry.category);
                    free(entry.activity);
                    free(entry.message);
                    
                    goto cleanup;
                    break;

                case CMD_NONE:
                default:
                    break;
            }

            
            // Periodic save every 60 seconds
            time_t now = time(NULL);
            if (now - last_save >= 60) {
                set_entry_elapsed_completed(&entry, timer);
                save_temp_entry(&entry);
                last_save = now;
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

        // Update entry with final state BEFORE saving
        set_entry_elapsed_completed(&entry, timer);
        
        // Append to history and remove temp
        if (append_entry(&entry)) {
            remove(TEMP_FILE);
            // Export to CSV after each timer
            export_entries_csv("data/entries.dat", "history.csv");
        }
        
        // Free entry strings
        free(entry.category);
        free(entry.activity);
        free(entry.message);

        // Play sound and wait for input (if not last phase)
        if (j < settings->countdown.num_cycles * 2) {
            printf("\a");
            fflush(stdout);
            
            int flags = fcntl(STDIN_FILENO, F_GETFL);
            fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
            
            while (1) {
                Command cmd = read_command();

                if (cmd == CMD_CONTINUE) {
                    break;
                } else if (cmd == CMD_QUIT) {
                    free(timer);
                    goto cleanup;
                }
            }
            
            fcntl(STDIN_FILENO, F_SETFL, flags);
        }
        
        free(timer);
    }

cleanup:
    restore_terminal(&old_tio);
    printf("\033[?25h");
    free_config(settings);
    
    return 0;
}