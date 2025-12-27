#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <sys/stat.h>
#include <errno.h>

#include "../../include/global.h"
#include "../../include/data.h"
#include "../../include/timer.h"
#include "../../include/literals.h"

// Internal storage format - compact and machine-friendly
typedef struct {
    uint32_t id;                // Unique ID (generated on save, 0 for unsaved)
    char date[11];              // "YYYY-MM-DD"
    TimerMode mode;             // Enum (1 byte)
    TimerWorkMode work_mode;    // Enum (1 byte)
    char category[CATEGORY_SIZE];
    char activity[ACTIVITY_SIZE];
    char comment[COMMENT_SIZE];
    int duration_seconds;       // Total duration in seconds
    bool completed;             // Did the timer complete successfully?
} HistoryEntry;

// Human-readable format - for display/export/CSV
typedef struct {
    uint32_t id;
    char date[11];              // "YYYY-MM-DD"
    char mode[MODE_SIZE];       // "COUNTDOWN" / "STOPWATCH"
    char work_mode[MODE_SIZE];  // "WORK" / "BREAK" / "LONG BREAK"
    char category[CATEGORY_SIZE];
    char activity[ACTIVITY_SIZE];
    char comment[COMMENT_SIZE];
    char duration[16];          // "25:30"
    int completed;              // 0 or 1
} HistoryEntryDisplay;

// Get current date as string in YYYY-MM-DD format
void get_current_date(char *date_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(date_str, size, "%04d-%02d-%02d", 
             t->tm_year + 1900, 
             t->tm_mon + 1, 
             t->tm_mday);
}

// Convert TimerMode enum to string
const char* timer_mode_to_string(TimerMode mode) {
    switch (mode) {
        case MODE_COUNTDOWN: return TIMER_COUNTDOWN;
        case MODE_STOPWATCH: return TIMER_STOPWATCH;
        default: return "UNKNOWN";
    }
}

// Convert TimerWorkMode enum to string
const char* timer_work_mode_to_string(TimerWorkMode work_mode) {
    switch (work_mode) {
        case MODE_WORK: return TIMER_WORK;
        case MODE_BREAK: return TIMER_BREAK;
        case MODE_LONG_BREAK: return TIMER_LONG_BREAK;
        default: return "UNKNOWN";
    }
}

// Create internal history entry from timer
HistoryEntry create_history_entry(
    const Timer *timer,
    const char* comment
) {
    HistoryEntry entry = {0};
    
    entry.id = 0;  // Will be assigned on save
    get_current_date(entry.date, sizeof(entry.date));
    
    entry.mode = timer->timer_mode;
    entry.work_mode = timer->timer_work_mode;
    
    strncpy(entry.category, timer->category, CATEGORY_SIZE - 1);
    entry.category[CATEGORY_SIZE - 1] = '\0';
    
    strncpy(entry.activity, timer->activity, ACTIVITY_SIZE - 1);
    entry.activity[ACTIVITY_SIZE - 1] = '\0';
    
    if (comment) {
        strncpy(entry.comment, comment, COMMENT_SIZE - 1);
        entry.comment[COMMENT_SIZE - 1] = '\0';
    }
    
    int64_t elapsed_ms = get_elapsed_ms(timer);
    entry.duration_seconds = (int)(elapsed_ms / 1000);
    entry.completed = (timer->timer_state == STATE_COMPLETED);

    return entry;
}

// Update an existing entry's duration (for periodic saves)
void update_entry_duration(HistoryEntry *entry, const Timer *timer) {
    int64_t elapsed_ms = get_elapsed_ms(timer);
    entry->duration_seconds = (int)(elapsed_ms / 1000);
    entry->completed = (timer->timer_state == STATE_COMPLETED);
}

// Convert internal entry to display format
HistoryEntryDisplay entry_to_display(const HistoryEntry *entry) {
    HistoryEntryDisplay display = {0};
    
    display.id = entry->id;
    
    // Copy basic fields
    strncpy(display.date, entry->date, sizeof(display.date) - 1);
    strncpy(display.category, entry->category, sizeof(display.category) - 1);
    strncpy(display.activity, entry->activity, sizeof(display.activity) - 1);
    strncpy(display.comment, entry->comment, sizeof(display.comment) - 1);
    
    // Convert enums to strings
    strncpy(display.mode, timer_mode_to_string(entry->mode), sizeof(display.mode) - 1);
    strncpy(display.work_mode, timer_work_mode_to_string(entry->work_mode), sizeof(display.work_mode) - 1);
    
    // Format duration as "MM:SS"
    int minutes = entry->duration_seconds / 60;
    int seconds = entry->duration_seconds % 60;
    snprintf(display.duration, sizeof(display.duration), "%02d:%02d", minutes, seconds);
    
    // Completed as 0/1
    display.completed = entry->completed ? 1 : 0;
    
    return display;
}

// Format entry as CSV line
void entry_to_csv(const HistoryEntry *entry, char *buffer, size_t size) {
    int minutes = entry->duration_seconds / 60;
    int seconds = entry->duration_seconds % 60;
    
    snprintf(buffer, size, "%u,%s,%s,%s,%s,%s,%s,%02d:%02d,%d\n",
             entry->id,
             entry->date,
             timer_mode_to_string(entry->mode),
             timer_work_mode_to_string(entry->work_mode),
             entry->category,
             entry->activity,
             entry->comment,
             minutes, seconds,
             entry->completed ? 1 : 0);
}

// Storage functions

// Save temporary entry (overwrites if exists) - for crash recovery
bool save_temp_entry(const HistoryEntry *entry) {
    FILE *f = fopen("data/.temp_entry", "wb");
    if (!f) return false;
    
    size_t written = fwrite(entry, sizeof(HistoryEntry), 1, f);
    fclose(f);
    
    return written == 1;
}

// Load temporary entry
bool load_temp_entry(HistoryEntry *entry) {
    FILE *f = fopen("data/.temp_entry", "rb");
    if (!f) return false;
    
    size_t read = fread(entry, sizeof(HistoryEntry), 1, f);
    fclose(f);
    
    return read == 1;
}

// Delete temporary entry
void delete_temp_entry(void) {
    remove("data/.temp_entry");
}

// Create data directory if it doesn't exist
static void ensure_data_directory(void) {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        mkdir("data", 0755);
    }
}

// Append entry to history file (assigns ID)
bool append_to_history(HistoryEntry *entry) {
    ensure_data_directory();
    
    // Read last ID from file or start at 1
    uint32_t next_id = 1;
    FILE *f = fopen("data/history.dat", "rb");
    if (f) {
        fseek(f, -sizeof(HistoryEntry), SEEK_END);
        HistoryEntry last;
        if (fread(&last, sizeof(HistoryEntry), 1, f) == 1) {
            next_id = last.id + 1;
        }
        fclose(f);
    }
    
    // Assign ID and append
    entry->id = next_id;
    
    f = fopen("data/history.dat", "ab");  // 'a' creates if doesn't exist
    if (!f) return false;
    
    size_t written = fwrite(entry, sizeof(HistoryEntry), 1, f);
    fclose(f);
    
    return written == 1;
}