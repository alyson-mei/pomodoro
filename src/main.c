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

#define DATA_DIR "data"
#define ENTRIES_FILE "data/entries.dat"
#define INDEX_FILE "data/entries.idx"
#define TEMP_FILE "data/.temp_entry"


// TODO:
// - Short term:
//      - Long break logic (e.g. should be 4-th instead of 5-th)
//      - Bug in duration logic
//      - Update CSV after each timer
//      - Add a message after a session
//      - Sessions logic (outer loop)
// - Long term:
//      - Category and activity should be on different lines
//      - Edit entries

// Ensure data directory exists
static void ensure_data_dir(void) {
    struct stat st = {0};
    if (stat(DATA_DIR, &st) == -1) {
        mkdir(DATA_DIR, 0755);
    }
}

// Save temporary entry (for crash recovery)
static bool save_temp_entry(const HistoryEntry *entry) {
    FILE *f = fopen(TEMP_FILE, "wb");
    if (!f) return false;
    
    bool ok = write_entry(f, entry);
    fclose(f);
    return ok;
}

// Load temporary entry
static bool load_temp_entry(HistoryEntry *entry) {
    FILE *f = fopen(TEMP_FILE, "rb");
    if (!f) return false;
    
    bool ok = read_entry(f, entry);
    fclose(f);
    return ok;
}

// Append entry to history
static bool append_entry(HistoryEntry *entry) {
    FILE *f_entries = fopen(ENTRIES_FILE, "ab");
    FILE *f_index = fopen(INDEX_FILE, "ab");
    
    if (!f_entries || !f_index) {
        if (f_entries) fclose(f_entries);
        if (f_index) fclose(f_index);
        return false;
    }
    
    bool ok = write_entry_index(f_entries, f_index, entry);
    
    fclose(f_entries);
    fclose(f_index);
    
    return ok;
}

// Check for crashed session on startup
static void check_crashed_session(void) {
    HistoryEntry entry;
    if (load_temp_entry(&entry)) {
        printf("\n=== Recovered incomplete session ===\n");
        printf("Date: %s %s\n", entry.date, entry.time);
        printf("Activity: %s -> %s\n", 
               entry.category ? entry.category : "N/A",
               entry.activity ? entry.activity : "N/A");
        printf("Duration: %d seconds (%s)\n", 
               entry.elapsed_seconds,
               entry.completed ? "completed" : "interrupted");
        printf("\nSave to history? (y/n): ");
        
        char response;
        scanf(" %c", &response);
        
        if (response == 'y' || response == 'Y') {
            if (append_entry(&entry)) {
                printf("Saved successfully.\n");
            } else {
                printf("Failed to save.\n");
            }
        }
        
        // Free allocated strings
        free(entry.category);
        free(entry.activity);
        free(entry.message);
        
        // Remove temp file
        remove(TEMP_FILE);
        
        printf("Press Enter to continue...");
        while (getchar() != '\n');
        getchar();
    }
}



int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    
    ensure_data_dir();
    check_crashed_session();

    Settings *settings = load_config("config.ini");
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

    for (int j = 0; j < settings->countdown.num_cycles * 2 + 1; j++) {
        if (j < settings->countdown.num_cycles * 2) {
            if (j % 2 == 0) {
                work_mode = MODE_WORK;
                duration = settings->countdown.work_minutes;  // Changed here
                cycle = j / 2 + 1;
            }
            else {
                work_mode = MODE_BREAK;
                duration = settings->countdown.break_minutes;  // Changed here
                cycle = j / 2 + 1;
            }
        }
        else {
            work_mode = MODE_LONG_BREAK;
            duration = settings->countdown.long_break_minutes;  // Changed here
            cycle = j / 2;
        }

        Timer* timer = create_timer(
            duration,  // Changed here
            MODE_COUNTDOWN,
            work_mode,
            settings->activity.category,
            settings->activity.activity
        );


        // Create history entry for this timer
        HistoryEntry entry = create_history_entry(timer, NULL);
        save_temp_entry(&entry);
        last_save = time(NULL);

        start_timer(timer);

        while (timer->timer_state != STATE_COMPLETED && 
               timer->timer_state != STATE_CANCELLED) {
            
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
                    
                    // Update entry before saving
                    set_entry_elapsed_completed(&entry, timer);
                    save_temp_entry(&entry);
                    
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
                }
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

        // Update entry with final state
        set_entry_elapsed_completed(&entry, timer);
        
        // Append to history and remove temp
        if (append_entry(&entry)) {
            remove(TEMP_FILE);
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
                if (read(STDIN_FILENO, &c, 1) == 1) {
                    if (c == '\n' || c == '\r') {
                        break;
                    } else if (c == 'q' || c == 'Q') {
                        free(timer);
                        goto cleanup;
                    }
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