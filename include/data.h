#ifndef DATA_H
#define DATA_H

#include "timer.h"

#define MAX_CATEGORIES 64
#define MAX_ACTIVITIES 64

#define DAYS_IN_A_MONTHS 30.44

#define PROGRESS_YAML_PATH "../progress.yaml"
#define PROGRESS_BIN_PATH "../data/progress.bin"


typedef struct {
    uint64_t uuid;              
    time_t timestamp;
    char date[11];              // "YYYY-MM-DD"
    char time[6];               // "HH:MM"
    
    TimerMode mode;             
    TimerWorkMode work_mode;    
    
    int elapsed_seconds;       
    uint8_t completed; 
    uint8_t active;

    char* category;
    char* activity;
    char* message;
} HistoryEntry;

bool read_entry(
    FILE *f,
    HistoryEntry *entry
);
HistoryEntry create_history_entry(
    const Timer *timer,
    const char *message
);
bool write_entry(
    FILE *f, 
    const HistoryEntry *entry
);

bool write_entry_index(
    FILE *entries,
    FILE *index,
    HistoryEntry *entry
);

void set_entry_elapsed_completed(
    HistoryEntry *entry,
    const Timer *timer
);


#endif