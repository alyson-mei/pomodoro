#ifndef CONFIG_H
#define CONFIG_H

#include "global.h"

typedef struct {
    char category[CATEGORY_MAX_SIZE];
    char activity[ACTIVITY_MAX_SIZE];
} ActivitySettings;

typedef struct {
    int work_minutes;
    int break_minutes;
    int num_cycles;
    int long_break_minutes;
    int num_sessions;
} CountdownSettings;

typedef struct {
    char color_theme[KEY_CONFIG_MAX_SIZE];
    char borders_type[KEY_CONFIG_MAX_SIZE];
} UiSettings;

typedef struct {
    ActivitySettings  activity;
    CountdownSettings countdown;
    UiSettings        ui;
} Settings;

Settings* load_config(const char *path);
void free_config(Settings *settings);

#endif