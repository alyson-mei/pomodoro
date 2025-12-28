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

const char* timer_mode_to_string(TimerMode mode) {
    switch (mode) {
        case MODE_COUNTDOWN: return TIMER_COUNTDOWN;
        case MODE_STOPWATCH: return TIMER_STOPWATCH;
        default: return "UNKNOWN";
    }
}

const char* timer_work_mode_to_string(TimerWorkMode work_mode) {
    switch (work_mode) {
        case MODE_WORK: return TIMER_WORK;
        case MODE_BREAK: return TIMER_BREAK;
        case MODE_LONG_BREAK: return TIMER_LONG_BREAK;
        default: return "UNKNOWN";
    }
}

static void csv_print_elapsed(FILE *out, int elapsed_seconds) {
    int minutes = elapsed_seconds / 60;
    int seconds = elapsed_seconds % 60;
    fprintf(out, "%02d:%02d", minutes, seconds);
}

static void csv_print_string(FILE *out, const char *s) {
    if (!s) {
        fputs("", out);
        return;
    }

    bool need_quotes = false;
    for (const char *p = s; *p; ++p) {
        if (*p == ',' || *p == '"' || *p == '\n') {
            need_quotes = true;
            break;
        }
    }

    if (!need_quotes) {
        fputs(s, out);
        return;
    }

    fputc('"', out);
    for (const char *p = s; *p; ++p) {
        if (*p == '"') fputc('"', out); // escape quote
        fputc(*p, out);
    }
    fputc('"', out);
}

// CSV export

bool export_entries_csv(const char *entries_path, const char *csv_path) {
    FILE *f_entries = fopen(entries_path, "rb");
    if (!f_entries) {
        perror("fopen entries");
        return false;
    }

    FILE *f_csv = fopen(csv_path, "w");
    if (!f_csv) {
        perror("fopen csv");
        fclose(f_entries);
        return false;
    }

    // CSV header (skip timestamp and active)
    fprintf(f_csv,
        "uuid,date,time,mode,work_mode,elapsed_seconds,completed,category,activity,message\n"
    );

    HistoryEntry entry;
    while (read_entry(f_entries, &entry)) {
        fprintf(f_csv, "%llu,", (unsigned long long)entry.uuid);
        fprintf(f_csv, "%s,%s,", entry.date, entry.time);
        fprintf(f_csv, "%s,%s,",
            timer_mode_to_string(entry.mode),
            timer_work_mode_to_string(entry.work_mode));
        csv_print_elapsed(f_csv, entry.elapsed_seconds);
        fprintf(f_csv, ",%u,", entry.completed);

        csv_print_string(f_csv, entry.category); fputc(',', f_csv);
        csv_print_string(f_csv, entry.activity); fputc(',', f_csv);
        csv_print_string(f_csv, entry.message); fputc('\n', f_csv);

        // free allocated strings
        free(entry.category);
        free(entry.activity);
        free(entry.message);
    }

    fclose(f_entries);
    fclose(f_csv);
    return true;
}

int main() {
    if (!export_entries_csv("data/entries.dat", "history.csv")) {
        fprintf(stderr, "Failed to export CSV\n");
    }
    return 0;
}