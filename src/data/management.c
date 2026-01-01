#define _GNU_SOURCE

#include <stdio.h>  
#include <stdlib.h>    
#include <stdbool.h>   
#include <stdint.h>     
#include <sys/stat.h>  
#include <unistd.h>

#include "../../include/global.h"
#include "../../include/data.h"

#define DATA_DIR "data"
#define ENTRIES_FILE "data/entries.dat"
#define INDEX_FILE "data/entries.idx"
#define TEMP_FILE "data/.temp_entry"

void ensure_data_dir(void) {
    struct stat st = {0};
    if (stat(DATA_DIR, &st) == -1) {
        mkdir(DATA_DIR, 0755);
    }
}

// Save temporary entry (for crash recovery)
bool save_temp_entry(const HistoryEntry *entry) {
    FILE *f = fopen(TEMP_FILE, "wb");
    if (!f) return false;
    
    bool ok = write_entry(f, entry);
    fclose(f);
    return ok;
}

// Load temporary entry
bool load_temp_entry(HistoryEntry *entry) {
    FILE *f = fopen(TEMP_FILE, "rb");
    if (!f) return false;
    
    bool ok = read_entry(f, entry);
    fclose(f);
    return ok;
}

// Append entry to history
bool append_entry(HistoryEntry *entry) {
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
void check_crashed_session(void) {
    HistoryEntry entry;
    if (load_temp_entry(&entry)) {
        // Only process if elapsed time is at least 60 seconds
        if (entry.elapsed_seconds >= 60) {
            // Silently save to history
            if (append_entry(&entry)) {
                export_entries_csv("data/entries.dat", "history.csv");
            }
        }
        
        // Free allocated strings
        free(entry.category);
        free(entry.activity);
        free(entry.message);
        
        // Always remove temp file
        remove(TEMP_FILE);
    }
}
