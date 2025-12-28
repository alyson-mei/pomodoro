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


typedef struct {
    uint32_t id;
    char date[11];              // "YYYY-MM-DD"
    TimerMode mode;             
    TimerWorkMode work_mode;    
    char* category;
    char* activity;
    char* comment;
    int duration_seconds;       
    bool completed;             
} HistoryEntry;

// Helpers

void get_current_date(char *date_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(
        date_str,
        size,
        "%04d-%02d-%02d",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday
    );
}

void set_entry_duration(
    HistoryEntry *entry,
    const Timer *timer
) {
    int64_t elapsed_ms = get_elapsed_ms(timer);
    entry->duration_seconds = elapsed_ms / 1000;
    entry->completed = (timer->timer_state == STATE_COMPLETED);
}

static bool write_string(FILE *f, const char *s) {
    uint32_t len = s ? (uint32_t)strlen(s) + 1 : 0;

    if (fwrite(&len, sizeof(len), 1, f) != 1)
        return false;

    if (len > 0 && fwrite(s, 1, len, f) != len)
        return false;

    return true;
}

static bool read_string(FILE *f, char **out) {
    uint32_t len;

    if (fread(&len, sizeof(len), 1, f) != 1)
        return false;

    if (len == 0) {
        *out = NULL;
        return true;
    }

    char *buf = malloc(len);
    if(!buf)
        return false;

    if (fread(buf, 1, len, f) != len) {
        free(buf);
        return false;
    }

    *out = buf;
    return true;
}


// Create, save and read


HistoryEntry create_history_entry(
    const Timer *timer,
    const char *comment
) {
    HistoryEntry entry = {0};

    entry.id = 0;
    get_current_date(entry.date, sizeof(entry.date));

    entry.mode = timer->timer_mode;
    entry.work_mode = timer->timer_work_mode;

    entry.category = timer->category ? strdup(timer->category) : NULL;
    entry.activity = timer->activity ? strdup(timer->activity) : NULL;
    entry.comment  = comment ? strdup(comment) : NULL;

    set_entry_duration(&entry, timer);

    return entry;
}

bool write_entry(FILE *f, const HistoryEntry *entry) {
    if (fwrite(&entry->id, sizeof(entry->id), 1, f) != 1)
        return false;
    if (fwrite(entry->date, sizeof(entry->date), 1, f) != 1)
        return false;
    if (fwrite(&entry->mode, sizeof(entry->mode), 1, f) != 1)
        return false;
    if (fwrite(&entry->work_mode, sizeof(entry->work_mode), 1, f) != 1)
        return false;
    if (fwrite(&entry->duration_seconds, sizeof(entry->duration_seconds), 1, f) != 1)
        return false;
    if (fwrite(&entry->completed, sizeof(entry->completed), 1, f) != 1)
        return false;

    if (!write_string(f, entry->category))
        return false;
    if (!write_string(f, entry->activity))
        return false;
    if (!write_string(f, entry->comment))
        return false;

    return true;
}

bool read_entry(FILE *f, HistoryEntry *entry) {
    memset(entry, 0, sizeof(*entry));

    if (fread(&entry->id, sizeof(entry->id), 1, f) != 1) 
        return false;
    if (fread(entry->date, sizeof(entry->date), 1, f) != 1) 
        return false;
    if (fread(&entry->mode, sizeof(entry->mode), 1, f) != 1) 
        return false;
    if (fread(&entry->work_mode, sizeof(entry->work_mode), 1, f) != 1) 
        return false;
    if (fread(&entry->duration_seconds, sizeof(entry->duration_seconds), 1, f) != 1) 
        return false;
    if (fread(&entry->completed, sizeof(entry->completed), 1, f) != 1)
        return false;

    if (!read_string(f, &entry->category)) return false;
    if (!read_string(f, &entry->activity)) return false;
    if (!read_string(f, &entry->comment))  return false;

    return true;       

}

bool write_entry_index(FILE *entries, FILE *index, HistoryEntry *entry) {
    if (fseek(index, 0, SEEK_END) != 0) return false;
    long index_pos = ftell(index);
    if (index_pos < 0) return false;
    
    entry->id = (uint32_t)(index_pos / sizeof(uint64_t));

    if (fseek(entries, 0, SEEK_END) != 0) return false;
    long data_pos = ftell(entries);
    if (data_pos < 0) return false;
    
    uint64_t offset = (uint64_t)data_pos;

    if (!write_entry(entries, entry)) return false;
    if (fwrite(&offset, sizeof(offset), 1, index) != 1) return false;

    return true;
}

bool build_index(
    const char *entries_path,
    const char *index_path
) {
    FILE *entries = fopen(entries_path, "rb");
    if (!entries) return false;

    FILE *index = fopen(index_path, "wb");
    if (!index) {
        fclose(entries);
        return false;
    }

    HistoryEntry entry;
    while (1) {
        long offset = ftell(entries);
        if (offset < 0) break;

        if (!read_entry(entries, &entry)) break;

        uint64_t uoffset = (uint64_t)offset;
        if (fwrite(&uoffset, sizeof(uoffset), 1, index) != 1) {
            fclose(entries);
            fclose(index);
            return false;
        }

        // Free strings if read_entry allocated them
        free(entry.category);
        free(entry.activity);
        free(entry.comment);
    }

    fclose(entries);
    fclose(index);
    return true;
}

int main() {
    Timer* timer = create_timer(
        10,
        MODE_COUNTDOWN,
        MODE_WORK,
        "category",
        "activity"
    );
    start_timer(timer);
    HistoryEntry entry = create_history_entry(timer, "comment");

    FILE *f_entries = fopen("src/experimental/exp_data.dat", "ab");
    if (!f_entries) {
        perror("fopen");
        return 1;
    }

    FILE *f_index = fopen("src/experimental/exp_data.idx", "ab");
    if (!f_index) {
        perror("fopen");
        return 1;
    }

    //write_entry_index(f_entries, f_index, &entry);

    // build_index("src/experimental/exp_data.dat", "src/experimental/exp_data.idx");

    FILE *f_entries_r = fopen("src/experimental/exp_data.dat", "rb");
    if (!f_entries_r) {
        perror("fopen");
        return 1;
    }

    // HistoryEntry entry;

    while (read_entry(f_entries_r, &entry)) {
        printf(
            "id=%u date=%s cat=%s act=%s comment=%s dur=%d completed=%d\n",
            entry.id,
            entry.date,
            entry.category,
            entry.activity,
            entry.comment,
            entry.duration_seconds,
            entry.completed
        );

        free(entry.category);
        free(entry.activity);
        free(entry.comment);
    }


    fclose(f_entries_r);
    fclose(f_index);
    return 0;
}
