#ifndef STATS_H
#define STATS_H

#include <stdint.h>
#include "data.h"

typedef struct {
    char name[ACTIVITY_SIZE];
    int64_t total_seconds;
} ActivityStats;

typedef struct {
    char name[CATEGORY_SIZE];
    ActivityStats activities[MAX_ACTIVITIES];
    size_t activity_count;
    int64_t total_seconds;
} CategoryStats;

typedef struct {
    CategoryStats categories[MAX_CATEGORIES];
    size_t category_count;
    int64_t total_seconds;
} StatsData;

// Build stats from entries file
bool build_stats_from_entries(const char *entries_path, StatsData *stats);

// Export functions
bool export_stats_yaml(const StatsData *stats, const char *output_path);
bool export_stats_csv(const StatsData *stats, const char *output_path);

// Helper to format duration
char* format_duration_seconds(int64_t seconds, char *buf, size_t bufsize, int truncate_zero);

#endif
