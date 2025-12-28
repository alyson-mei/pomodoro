#define _GNU_SOURCE
#include <unistd.h>
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


// Helpers

uint64_t generate_uuid(void) {
    static uint64_t counter = 0;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    uint64_t t = ((uint64_t)ts.tv_sec << 32) | (ts.tv_nsec & 0xffffffff);
    return t ^ (++counter);
}

void set_entry_time(HistoryEntry *entry) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    entry->timestamp = now;

    snprintf(
        entry->date,
        sizeof(entry->date),
        "%04d-%02d-%02d",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday
    );

    snprintf(
        entry->time,
        sizeof(entry->time),
        "%02d:%02d",
        t->tm_hour,
        t->tm_min
    );
}

void set_entry_elapsed_completed(
    HistoryEntry *entry,
    const Timer *timer
) {
    int64_t elapsed_ms = get_elapsed_ms(timer);
    entry->elapsed_seconds = elapsed_ms / 1000;
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

static bool read_string(
    FILE *f, 
    char **out
) {
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
    const char *message
) {
    HistoryEntry entry = {0};

    entry.uuid = 0;
    set_entry_time(&entry);

    entry.mode = timer->timer_mode;
    entry.work_mode = timer->timer_work_mode;
    set_entry_elapsed_completed(&entry, timer);   //
    entry.active = 1;

    entry.category = timer->category ? strdup(timer->category) : NULL;
    entry.activity = timer->activity ? strdup(timer->activity) : NULL;
    entry.message  = message ? strdup(message) : NULL;


    return entry;
}

bool write_entry(
    FILE *f, 
    const HistoryEntry *entry
) {
    if (fwrite(&entry->uuid, sizeof(entry->uuid), 1, f) != 1)
        return false;
    if (fwrite(entry->date, sizeof(entry->date), 1, f) != 1)
        return false;
    if (fwrite(entry->time, sizeof(entry->time), 1, f) != 1)
        return false;
    if (fwrite(&entry->timestamp, sizeof(entry->timestamp), 1, f) != 1)
        return false;
        
    if (fwrite(&entry->mode, sizeof(entry->mode), 1, f) != 1)
        return false;
    if (fwrite(&entry->work_mode, sizeof(entry->work_mode), 1, f) != 1)
        return false;

    if (fwrite(&entry->elapsed_seconds, sizeof(entry->elapsed_seconds), 1, f) != 1)
        return false;
    if (fwrite(&entry->completed, sizeof(entry->completed), 1, f) != 1)
        return false;
    if (fwrite(&entry->active, sizeof(entry->active), 1, f) != 1)
        return false;

    if (!write_string(f, entry->category))
        return false;
    if (!write_string(f, entry->activity))
        return false;
    if (!write_string(f, entry->message))
        return false;

    return true;
}

bool read_entry(FILE *f, HistoryEntry *entry) {
    memset(entry, 0, sizeof(*entry));

    if (fread(&entry->uuid, sizeof(entry->uuid), 1, f) != 1) 
        return false;
    if (fread(entry->date, sizeof(entry->date), 1, f) != 1) 
        return false;
    if (fread(entry->time, sizeof(entry->time), 1, f) != 1) 
        return false;
    if (fread(&entry->timestamp, sizeof(entry->timestamp), 1, f) != 1) 
        return false;

    if (fread(&entry->mode, sizeof(entry->mode), 1, f) != 1) 
        return false;
    if (fread(&entry->work_mode, sizeof(entry->work_mode), 1, f) != 1) 
        return false;

    if (fread(&entry->elapsed_seconds, sizeof(entry->elapsed_seconds), 1, f) != 1) 
        return false;
    if (fread(&entry->completed, sizeof(entry->completed), 1, f) != 1)
        return false;
    if (fread(&entry->active, sizeof(entry->active), 1, f) != 1)
        return false;

    if (!read_string(f, &entry->category)) return false;
    if (!read_string(f, &entry->activity)) return false;
    if (!read_string(f, &entry->message))  return false;

    return true;       
}

bool write_entry_index(
    FILE *entries,
    FILE *index,
    HistoryEntry *entry
) {
    if (entry->uuid == 0)
        entry->uuid = generate_uuid();

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
        free(entry.message);
    }

    fclose(entries);
    fclose(index);
    return true;
}




// int main(void) {
//     const char *entries_path = "src/experimental/exp_data.dat";
//     const char *index_path   = "src/experimental/exp_data.idx";

//     // --- clean start ---
//     unlink(entries_path);
//     unlink(index_path);

//     // --- create timer ---
//     Timer *timer = create_timer(
//         10,
//         MODE_COUNTDOWN,
//         MODE_WORK,
//         "category",
//         "activity"
//     );
//     start_timer(timer);

//     // --- open files ---
//     FILE *f_entries = fopen(entries_path, "ab");
//     if (!f_entries) {
//         perror("fopen entries");
//         return 1;
//     }

//     FILE *f_index = fopen(index_path, "ab");
//     if (!f_index) {
//         perror("fopen index");
//         fclose(f_entries);
//         return 1;
//     }

//     // --- write entries ---
//     for (int i = 0; i < 3; i++) {
//         HistoryEntry entry = create_history_entry(timer, "message");

//         if (!write_entry_index(f_entries, f_index, &entry)) {
//             fprintf(stderr, "write_entry_index failed\n");
//             return 1;
//         }

//         // free owned strings
//         free(entry.category);
//         free(entry.activity);
//         free(entry.message);
//     }

//     fclose(f_entries);
//     fclose(f_index);

//     // --- read back entries ---
//     FILE *f_entries_r = fopen(entries_path, "rb");
//     if (!f_entries_r) {
//         perror("fopen read");
//         return 1;
//     }

//     HistoryEntry entry;
//     while (read_entry(f_entries_r, &entry)) {
//         printf(
//             "uuid=%llu date=%s time=%s cat=%s act=%s msg=%s dur=%d completed=%d active=%d\n",
//             (unsigned long long)entry.uuid,
//             entry.date,
//             entry.time,
//             entry.category ? entry.category : "(null)",
//             entry.activity ? entry.activity : "(null)",
//             entry.message ? entry.message : "(null)",
//             entry.elapsed_seconds,
//             entry.completed,
//             entry.active
//         );

//         free(entry.category);
//         free(entry.activity);
//         free(entry.message);
//     }

//     fclose(f_entries_r);

//     return 0;
// }
