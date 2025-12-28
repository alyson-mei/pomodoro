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

void update_entry_duration(
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

    update_entry_duration(&entry, timer);

    return entry;
}


bool save_entry(
    const HistoryEntry *entry,
    const char *path,
    const char *mode
) {
    bool ok = false;
    FILE *f = fopen(path, mode);
    if (!f) return false;

    do {
        if (fwrite(&entry->id, sizeof(entry->id), 1, f) != 1) break;
        if (fwrite(entry->date, sizeof(entry->date), 1, f) != 1) break;
        if (fwrite(&entry->mode, sizeof(entry->mode), 1, f) != 1) break;
        if (fwrite(&entry->work_mode, sizeof(entry->work_mode), 1, f) != 1) break;
        if (fwrite(&entry->duration_seconds, sizeof(entry->duration_seconds), 1, f) != 1) break;
        if (fwrite(&entry->completed, sizeof(entry->completed), 1, f) != 1) break;

        if (!write_string(f, entry->category)) break;
        if (!write_string(f, entry->activity)) break;
        if (!write_string(f, entry->comment)) break;

        ok = true;
    } while (0);

    fclose(f);
    return ok;
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

int main() {
    // Timer* timer = create_timer(
    //     10,
    //     MODE_COUNTDOWN,
    //     MODE_WORK,
    //     "category",
    //     "activity"
    // );
    // start_timer(timer);

    // HistoryEntry entry = create_history_entry(timer, "comment");
    // save_entry(&entry, "exp_data.dat", "ab");


    FILE *f = fopen("exp_data.dat", "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    HistoryEntry entry;

    while (read_entry(f, &entry)) {
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


    fclose(f);
    return 0;
}
