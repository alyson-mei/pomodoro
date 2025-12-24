#ifndef GLOBAL_H
#define GLOBAL_H

#define BUF_SIZE_XXS    16
#define BUF_SIZE_XS     32
#define BUF_SIZE_S      64
#define BUF_SIZE_M      128
#define BUF_SIZE_L      256

// Define size constants first
enum {
    STR_CONFIG_MAX_SIZE    = BUF_SIZE_L,
    KEY_CONFIG_MAX_SIZE    = BUF_SIZE_XS,  
    CATEGORY_MAX_SIZE      = BUF_SIZE_M,
    ACTIVITY_MAX_SIZE      = BUF_SIZE_L
};

// Now use them in structs
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
} UISettings;

typedef struct {
    ActivitySettings  activity;
    CountdownSettings countdown;
    UISettings        ui;
} Settings;

Settings* load_config(const char *path);
void free_config(Settings *settings);

#endif