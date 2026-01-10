#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../include/stats.h"
#include "../../include/data.h"
#include "../../include/global.h"

char* format_duration_seconds(int64_t total_seconds, char *buf, size_t bufsize, int truncate_zero) {
    int seconds = total_seconds % 60;
    int64_t total_minutes = total_seconds / 60;
    int minutes = total_minutes % 60;
    int64_t total_hours = total_minutes / 60;
    int hours = total_hours % 24;
    int64_t total_days = total_hours / 24;
    int days = total_days % 30;
    int64_t total_months = total_days / 30;
    int months = total_months % 12;
    int years = total_months / 12;

    size_t len = 0;
    if (!truncate_zero || years)   len += snprintf(buf + len, bufsize - len, "%dy ", years);
    if (!truncate_zero || months)  len += snprintf(buf + len, bufsize - len, "%dmo ", months);
    if (!truncate_zero || days)    len += snprintf(buf + len, bufsize - len, "%dd ", days);
    if (!truncate_zero || hours)   len += snprintf(buf + len, bufsize - len, "%dh ", hours);
    if (!truncate_zero || minutes) len += snprintf(buf + len, bufsize - len, "%dm ", minutes);
    if (!truncate_zero || seconds) len += snprintf(buf + len, bufsize - len, "%ds", seconds);

    if (len > 0 && buf[len-1] == ' ') buf[len-1] = '\0';
    return buf;
}

static void add_time_to_stats(StatsData *stats, const char *category_name, const char *activity_name, int64_t seconds) {
    // Handle empty/NULL category
    const char *cat_name = (category_name && strlen(category_name) > 0) ? category_name : "(no category)";
    const char *act_name = (activity_name && strlen(activity_name) > 0) ? activity_name : "(no activity)";

    // Find or create category
    CategoryStats *cat = NULL;
    for (size_t i = 0; i < stats->category_count; i++) {
        if (strncmp(stats->categories[i].name, cat_name, CATEGORY_SIZE) == 0) {
            cat = &stats->categories[i];
            break;
        }
    }

    if (!cat) {
        if (stats->category_count >= MAX_CATEGORIES) return;
        cat = &stats->categories[stats->category_count++];
        strncpy(cat->name, cat_name, CATEGORY_SIZE - 1);
        cat->name[CATEGORY_SIZE - 1] = '\0';
        cat->activity_count = 0;
        cat->total_seconds = 0;
    }

    // Find or create activity
    ActivityStats *act = NULL;
    for (size_t j = 0; j < cat->activity_count; j++) {
        if (strncmp(cat->activities[j].name, act_name, ACTIVITY_SIZE) == 0) {
            act = &cat->activities[j];
            break;
        }
    }

    if (!act) {
        if (cat->activity_count >= MAX_ACTIVITIES) return;
        act = &cat->activities[cat->activity_count++];
        strncpy(act->name, act_name, ACTIVITY_SIZE - 1);
        act->name[ACTIVITY_SIZE - 1] = '\0';
        act->total_seconds = 0;
    }

    // Update totals
    stats->total_seconds += seconds;
    cat->total_seconds += seconds;
    act->total_seconds += seconds;
}

bool build_stats_from_entries(const char *entries_path, StatsData *stats) {
    memset(stats, 0, sizeof(*stats));

    FILE *f = fopen(entries_path, "rb");
    if (!f) {
        fprintf(stderr, "Could not open entries file: %s\n", entries_path);
        return false;
    }

    HistoryEntry entry;
    while (read_entry(f, &entry)) {
        add_time_to_stats(stats, entry.category, entry.activity, entry.elapsed_seconds);
        
        // Free entry strings
        free(entry.category);
        free(entry.activity);
        free(entry.message);
    }

    fclose(f);
    return true;
}

bool export_stats_yaml(const StatsData *stats, const char *output_path) {
    char buf[256];
    
    FILE *f = fopen(output_path, "w");
    if (!f) {
        fprintf(stderr, "Could not open output file: %s\n", output_path);
        return false;
    }

    fprintf(f, "# Statistics Summary\n");
    format_duration_seconds(stats->total_seconds, buf, sizeof(buf), 1);
    fprintf(f, "total: %s\n\n", buf);

    for (size_t i = 0; i < stats->category_count; i++) {
        const CategoryStats *cat = &stats->categories[i];
        format_duration_seconds(cat->total_seconds, buf, sizeof(buf), 1);
        fprintf(f, "%s:\n  total: %s\n", cat->name, buf);

        for (size_t j = 0; j < cat->activity_count; j++) {
            const ActivityStats *act = &cat->activities[j];
            format_duration_seconds(act->total_seconds, buf, sizeof(buf), 1);
            fprintf(f, "  - %s: %s\n", act->name, buf);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return true;
}

bool export_stats_csv(const StatsData *stats, const char *output_path) {
    FILE *f = fopen(output_path, "w");
    if (!f) {
        fprintf(stderr, "Could not open output file: %s\n", output_path);
        return false;
    }

    fprintf(f, "category,activity,total_seconds\n");

    for (size_t i = 0; i < stats->category_count; i++) {
        const CategoryStats *cat = &stats->categories[i];

        for (size_t j = 0; j < cat->activity_count; j++) {
            const ActivityStats *act = &cat->activities[j];
            fprintf(f, "%s,%s,%ld\n", cat->name, act->name, (long)act->total_seconds);
        }
    }

    fclose(f);
    return true;
}