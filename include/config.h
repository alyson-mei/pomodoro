#ifndef CONFIG_H
#define CONFIG_H

#include "global.h"
#include "literals.h"
#include "ui/ui.h"

typedef struct {
    char category[CATEGORY_SIZE];
    char activity[ACTIVITY_SIZE];
} ActivitySettings;

typedef struct {
    int work_minutes;
    int break_minutes;
    int num_cycles;
    int long_break_minutes;
    int num_sessions;
} CountdownSettings;

typedef struct {
    ColorTheme color_theme;
    BorderTheme border_theme;
    ProgressTheme progress_theme;
} UiSettings;

typedef struct {
    ActivitySettings  activity;
    CountdownSettings countdown;
    UiSettings        ui;
} Settings;

Settings* load_config(const char *path);
void free_config(Settings *settings);

#endif