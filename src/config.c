#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/global.h"

// TODO: Use more consistent definitions for buffer sizes
// TODO: Aliases for categories/activities? Or some kind of fast search? 

typedef struct {
    char timer_mode[BUF_SIZE_XS];
    int work_minutes;
    int break_minutes;
    int long_break_minutes;
    char default_category[BUF_SIZE_M];
    char default_activity[BUF_SIZE_M];
} TimerSettings;

typedef struct {
    int work_min;
    int break_min;
} PresetCycle;

typedef struct {
    char name[BUF_SIZE_S];
    PresetCycle cycles[MAX_CYCLES_PER_PRESET];  
    int n_presets;
} PresetSettings;

typedef struct {
    char color_theme[BUF_SIZE_XS];
    char borders_type[BUF_SIZE_XS];
} UISettings;

typedef struct {
    TimerSettings timer;
    PresetSettings presets[MAX_PRESETS];
    int n_presets;
    UISettings ui;
} Settings;


// Trim leading and trailing whitespace
static char* trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) 
        str++;
    
    if(*str == 0) return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) 
        end--;

    end[1] = '\0';
    return str;
}

static int parse_line(const char *line, char *key, char *value) {
    const char *eq = strchr(line, '=');
    if (!eq) return 0;

    // Extract key
    size_t key_len = eq - line;
    if (key_len >= BUF_SIZE_S) key_len = BUF_SIZE_S - 1;
    strncpy(key, line, key_len);
    key[key_len] = '\0';

    // Extract value
    const char *val_start = eq + 1;
    strncpy(value, val_start, BUF_SIZE_L - 1);
    value[BUF_SIZE_L - 1] = '\0';

    // Trim both
    char *trimmed_key = trim(key);
    char *trimmed_val = trim(value);

    if (trimmed_key != key) memmove(key, trimmed_key, strlen(trimmed_key) + 1);
    if (trimmed_val != value) memmove(value, trimmed_val, strlen(trimmed_val) + 1);

    return (key[0] != '\0');
}

// Parse timer settings
TimerSettings parse_timer(const char *str) {
    TimerSettings settings = {0}; 

    char line[BUF_SIZE_L];
    const char *ptr = str; 
    int in_timer_section = 0;

    while (*ptr != '\0') {
        int i = 0;
        while (*ptr != '\n' && *ptr != '\0' && i < BUF_SIZE_L - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';
        if (*ptr == '\n') ptr++;

        char *trimmed = trim(line);

        // Skip empty lines and comments
        if (trimmed[0] == '\0' || trimmed[0] == '#') continue;
        
        // Check for section headers
        if (trimmed[0] == '[') {
            in_timer_section = (strstr(trimmed, "[timer]") != NULL);
            continue;
        }

        if (!in_timer_section) continue;

        char key[BUF_SIZE_S], value[BUF_SIZE_L];
        if (parse_line(trimmed, key, value)) {
            if (strcmp(key, "timer_mode") == 0) {
                strncpy(settings.timer_mode, value, BUF_SIZE_XS - 1);
                settings.timer_mode[BUF_SIZE_XS - 1] = '\0';
            } else if (strcmp(key, "work_minutes") == 0) {
                settings.work_minutes = atoi(value);
            } else if (strcmp(key, "break_minutes") == 0) {
                settings.break_minutes = atoi(value);
            } else if (strcmp(key, "long_break_minutes") == 0) {
                settings.long_break_minutes = atoi(value);
            } else if (strcmp(key, "default_category") == 0) {
                strncpy(settings.default_category, value, BUF_SIZE_M - 1);
                settings.default_category[BUF_SIZE_M - 1] = '\0';
            } else if (strcmp(key, "default_activity") == 0) {
                strncpy(settings.default_activity, value, BUF_SIZE_M - 1);
                settings.default_activity[BUF_SIZE_M - 1] = '\0';
            }
        }
    }

    return settings;
}

// Parse preset value string like "25,5 | 25,5 | 25,15"
int parse_preset_value(const char *str, PresetCycle *cycles, int max_cycles) {
    int count = 0;
    char buf[BUF_SIZE_M];
    strncpy(buf, str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *token = strtok(buf, "|");
    while (token != NULL && count < max_cycles) {
        char *trimmed = trim(token);
    
        int work, brk;
        if (sscanf(trimmed, "%d , %d", &work, &brk) == 2) {
            cycles[count].work_min = work;
            cycles[count].break_min = brk;
            count++;
        }

        token = strtok(NULL, "|");
    }

    return count;
}

// Parse all presets from config string
int parse_presets(const char *str, PresetSettings *presets, int max_presets) {
    int preset_count = 0;
    char line[BUF_SIZE_L];
    const char *ptr = str;
    int in_presets_section = 0;

        while (*ptr != '\0' && preset_count < max_presets) {
        int i = 0;
        while (*ptr != '\n' && *ptr != '\0' && i < BUF_SIZE_L - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';
        if (*ptr == '\n') ptr++;

        char *trimmed = trim(line);
        
        if (trimmed[0] == '\0' || trimmed[0] == '#') continue;
        
        if (trimmed[0] == '[') {
            in_presets_section = (strstr(trimmed, "[presets]") != NULL);
            continue;
        }
        
        if (!in_presets_section) continue;

        char key[64], value[256];
        if (parse_line(trimmed, key, value)) {
            strncpy(presets[preset_count].name, key, 63);
            presets[preset_count].name[63] = '\0';
            
            presets[preset_count].n_presets = parse_preset_value(
                value, 
                presets[preset_count].cycles, 
                MAX_CYCLES_PER_PRESET
            );
            
            if (presets[preset_count].n_presets > 0) {
                preset_count++;
            }
        }
    }

    return preset_count;
}

// Parse UI settings
UISettings parse_ui(const char *str) {
    UISettings settings = {0};
    char line[BUF_SIZE_L];
    const char *ptr = str;
    int in_ui_section = 0;

    while (*ptr != '\0') {
        int i = 0;
        while (*ptr != '\n' && *ptr != '\0' && i < BUF_SIZE_L - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';
        if (*ptr == '\n') ptr++;

        char *trimmed = trim(line);
        
        if (trimmed[0] == '\0' || trimmed[0] == '#') continue;
        
        if (trimmed[0] == '[') {
            in_ui_section = (strstr(trimmed, "[ui]") != NULL);
            continue;
        }
        
        if (!in_ui_section) continue;

        char key[64], value[256];
        if (parse_line(trimmed, key, value)) {
            if (strcmp(key, "color_theme") == 0) {
                strncpy(settings.color_theme, value, BUF_SIZE_XS - 1);
                settings.color_theme[BUF_SIZE_XS - 1] = '\0';
            } else if (strcmp(key, "borders_type") == 0) {
                strncpy(settings.borders_type, value, BUF_SIZE_XS - 1);
                settings.borders_type[BUF_SIZE_XS - 1] = '\0';
            }
        }
    }

    return settings;
}


// Load and parse entire config file
Settings* load_config(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open config file '%s'\n", path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer and read entire file
    char *buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, size, file);
    buffer[bytes_read] = '\0';
    fclose(file);

    // Allocate Settings struct
    Settings *settings = (Settings*)calloc(1, sizeof(Settings));
    if (!settings) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(buffer);
        return NULL;
    }

    // Parse all sections
    settings->timer = parse_timer(buffer);
    settings->n_presets = parse_presets(buffer, settings->presets, MAX_PRESETS);
    settings->ui = parse_ui(buffer);

    free(buffer);
    return settings;
}

// Free settings allocated by load_config
void free_config(Settings *settings) {
    if (settings) {
        free(settings);
    }
}



// TODO: replace to tests 
// Test config loading
void test_file_loading() {
    Settings *settings = load_config("config.ini");
    if (!settings) {
        printf("Failed to load config\n");
        return;
    }

    printf("=== Settings loaded from file ===\n\n");
    
    printf("[Timer]\n");
    printf("  Mode: %s\n", settings->timer.timer_mode);
    printf("  Work: %d min, Break: %d min, Long break: %d min\n", 
           settings->timer.work_minutes, 
           settings->timer.break_minutes, 
           settings->timer.long_break_minutes);
    printf("  Category: %s\n", settings->timer.default_category);
    printf("  Activity: %s\n\n", settings->timer.default_activity);

    printf("[Presets] (%d total)\n", settings->n_presets);
    for (int i = 0; i < settings->n_presets; i++) {
        printf("  %s: ", settings->presets[i].name);
        for (int j = 0; j < settings->presets[i].n_presets; j++) {
            printf("%d,%d%s", 
                   settings->presets[i].cycles[j].work_min, 
                   settings->presets[i].cycles[j].break_min,
                   (j < settings->presets[i].n_presets - 1) ? " | " : "");
        }
        printf("\n");
    }
    printf("\n");

    printf("[UI]\n");
    printf("  Theme: %s\n", settings->ui.color_theme);
    printf("  Borders: %s\n", settings->ui.borders_type);

    free_config(settings);
}


// int main(){

//     test_file_loading();
//     return 0;
// }