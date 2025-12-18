#include <stdio.h>

typedef struct {
    char mode[32];
    int work_minutes;
    int break_minutes;
    int long_break_minutes;
    char default_category[256];
    char default_subcategory[256];
} TimerSettings;

typedef struct {
    int work_min;
    int break_min;
} PresetSettings;

typedef struct {
    PresetSettings *cycles;  // dynamically allocated array
    int n_cycles;
} Preset;

TimerSettings parse_timer(const char *str) {
    //Allocates settings struct on the stack with zero-initializing all fields
    TimerSettings settings = {0}; 

    char line[512];
    //We move ptr forward to scan through the string
    const char *ptr = str; 

    //Read line by line; '\0' is null terminator (end of string)
    while (*ptr != '\0') {
        //Extract one line
        int i = 0;
        while (*ptr != '\n' && *ptr != '\0' && i < 511) {
            line[i++] = *ptr++;
        }
        line[i] = '\0'; //Add null terminator to make line a valid C string
        if (*ptr == '\n') ptr++;

        if (line[0] == '[') continue;

        char key[64], value[256];
                char key[64], value[256];
        if (sscanf(line, "%[^ =] = %[^\n]", key, value) == 2) {
            if (strcmp(key, "mode") == 0) {
                strncpy(settings.mode, value, 31);
            } else if (strcmp(key, "work_minutes") == 0) {
                settings.work_minutes = atoi(value);
            } else if (strcmp(key, "break_minutes") == 0) {
                settings.break_minutes = atoi(value);
            } else if (strcmp(key, "long_break_minutes") == 0) {
                settings.long_break_minutes = atoi(value);
            } else if (strcmp(key, "default_category") == 0) {
                strncpy(settings.default_category, value, 255);
            } else if (strcmp(key, "default_subcategory") == 0) {
                strncpy(settings.default_subcategory, value, 255);
            }
        }
    }
}

int parse_preset(const char *str, PresetSettings *out, int max_cycles) {
    int count = 0;
    char buf[256];
    strncpy(buf, str, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';
}