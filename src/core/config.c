#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../../include/global.h"
#include "../../include/config.h"

// Trim leading and trailing whitespace
static char* trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) 
        str++;
    
    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) 
        end--;

    end[1] = '\0';
    return str;
}

// Parse a single key=value line
static int parse_line(const char *line, char *key, char *value) {
    const char *eq = strchr(line, '=');
    if (!eq) return 0;

    // Extract key
    size_t key_len = eq - line;
    if (key_len >= KEY_CONFIG_SIZE) key_len = KEY_CONFIG_SIZE - 1;
    strncpy(key, line, key_len);
    key[key_len] = '\0';

    // Extract value
    const char *val_start = eq + 1;
    strncpy(value, val_start, STR_CONFIG_SIZE - 1);
    value[STR_CONFIG_SIZE - 1] = '\0';

    // Trim both
    char *trimmed_key = trim(key);
    char *trimmed_val = trim(value);

    if (trimmed_key != key) memmove(key, trimmed_key, strlen(trimmed_key) + 1);
    if (trimmed_val != value) memmove(value, trimmed_val, strlen(trimmed_val) + 1);

    return (key[0] != '\0');
}

// Iterator callback function type
typedef void (*section_handler)(const char *key, const char *value, void *data);

// Generic section parser - calls handler for each key=value in a section
static void parse_section(const char *str, const char *section_name, 
                         section_handler handler, void *data) {
    char line[STR_CONFIG_SIZE];
    const char *ptr = str;
    int in_section = 0;

    while (*ptr != '\0') {
        int i = 0;
        while (*ptr != '\n' && *ptr != '\0' && i < STR_CONFIG_SIZE - 1) {
            line[i++] = *ptr++;
        }
        line[i] = '\0';
        if (*ptr == '\n') ptr++;

        char *trimmed = trim(line);

        // Skip empty lines and comments
        if (trimmed[0] == '\0' || trimmed[0] == '#') continue;
        
        // Check for section headers
        if (trimmed[0] == '[') {
            in_section = (strstr(trimmed, section_name) != NULL);
            continue;
        }

        if (!in_section) continue;

        char key[KEY_CONFIG_SIZE], value[STR_CONFIG_SIZE];
        if (parse_line(trimmed, key, value)) {
            handler(key, value, data);
        }
    }
}

// Helpers
static LayoutTheme get_layout_theme(const char *str) {
    if      (strcmp(str, LT_MINIMAL)   == 0) return LT_THEME_MINIMAL;
    else if (strcmp(str, LT_SMALL)    == 0)  return LT_THEME_SMALL;
    else if (strcmp(str, LT_STANDARD)  == 0) return LT_THEME_STANDARD;
    else                                     return LT_THEME_MINIMAL;
}

static ColorTheme get_color_theme(const char *str) {
    if      (strcmp(str, CT_MINIMAL)   == 0) return THEME_MINIMAL;
    else if (strcmp(str, CT_SIMPLE)    == 0) return THEME_SIMPLE;
    else if (strcmp(str, CT_CYBERPUNK) == 0) return THEME_CYBERPUNK;
    else if (strcmp(str, CT_FOREST)    == 0) return THEME_FOREST;
    else if (strcmp(str, CT_SUNSET)    == 0) return THEME_SUNSET;
    else                                     return THEME_MINIMAL;
}

static BorderTheme get_border_theme(const char *str) {
    if      (strcmp(str, BT_MINIMAL)   == 0) return BORDER_MINIMAL;
    else if (strcmp(str, BT_SINGLE)    == 0) return BORDER_SINGLE;
    else if (strcmp(str, BT_DOUBLE)    == 0) return BORDER_DOUBLE;
    else                                     return BORDER_MINIMAL;
}

static ProgressTheme get_progress_theme(const char *str) {
    if      (strcmp(str, PT_MINIMAL)   == 0) return PB_THEME_MINIMAL;
    else if (strcmp(str, PT_SIMPLE)    == 0) return PB_THEME_SIMPLE;
    else if (strcmp(str, PT_STANDARD)  == 0) return PB_THEME_STANDARD;
    else                                     return PB_THEME_MINIMAL;
}

// Handler for [activity] section
static void handle_activity(const char *key, const char *value, void *data) {
    ActivitySettings *settings = (ActivitySettings*)data;
    
    if (strcmp(key, "category") == 0) {
        strncpy(settings->category, value, CATEGORY_SIZE - 1);
        settings->category[CATEGORY_SIZE - 1] = '\0';
    } else if (strcmp(key, "activity") == 0) {
        strncpy(settings->activity, value, ACTIVITY_SIZE - 1);
        settings->activity[ACTIVITY_SIZE - 1] = '\0';
    }
}


// Handler for [countdown] section
static void handle_countdown(const char *key, const char *value, void *data) {
    CountdownSettings *settings = (CountdownSettings*)data;
    
    if (strcmp(key, "work_minutes") == 0) {
        settings->work_minutes = atoi(value);
    } else if (strcmp(key, "break_minutes") == 0) {
        settings->break_minutes = atoi(value);
    } else if (strcmp(key, "num_cycles") == 0) {
        settings->num_cycles = atoi(value);
    } else if (strcmp(key, "long_break_minutes") == 0) {
        settings->long_break_minutes = atoi(value);
    } else if (strcmp(key, "num_sessions") == 0) {
        settings->num_sessions = atoi(value);
    }
}

// Handler for [ui] section
static void handle_ui(const char *key, const char *value, void *data) {
    UiSettings *settings = (UiSettings*)data;
    
    if (strcmp(key, "layout_theme") == 0) {
        settings->layout_theme = get_layout_theme(value);
    } else if (strcmp(key, "color_theme") == 0) {
        settings->color_theme = get_color_theme(value);
    } else if (strcmp(key, "borders_theme") == 0) {
        settings->border_theme = get_border_theme(value);
    } else if (strcmp(key, "progress_theme") == 0) {
        settings->progress_theme = get_progress_theme(value);
    }
}

// Parse activity settings
ActivitySettings parse_activity(const char *str) {
    ActivitySettings settings = {0};
    parse_section(str, "[activity]", handle_activity, &settings);
    return settings;
}

// Parse countdown settings
CountdownSettings parse_countdown(const char *str) {
    CountdownSettings settings = {0};
    parse_section(str, "[countdown]", handle_countdown, &settings);
    return settings;
}

// Parse UI settings
UiSettings parse_ui(const char *str) {
    UiSettings settings = {0};
    parse_section(str, "[ui]", handle_ui, &settings);
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
    settings->activity = parse_activity(buffer);
    settings->countdown = parse_countdown(buffer);
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

// TODO: Replace with proper tests
// Test config loading
void test_file_loading() {
    Settings *settings = load_config("config.ini");
    if (!settings) {
        printf("Failed to load config\n");
        return;
    }

    printf("=== Settings loaded from file ===\n\n");
    
    printf("[Activity]\n");
    printf("  Category: %s\n", settings->activity.category);
    printf("  Activity: %s\n\n", settings->activity.activity);

    printf("[Countdown]\n");
    printf("  Work: %d min\n", settings->countdown.work_minutes);
    printf("  Break: %d min\n", settings->countdown.break_minutes);
    printf("  Cycles: %d\n", settings->countdown.num_cycles);
    printf("  Long break: %d min\n", settings->countdown.long_break_minutes);
    printf("  Sessions: %d\n\n", settings->countdown.num_sessions);

    printf("[UI]\n");
    printf("  Theme: %d\n", settings->ui.color_theme);
    printf("  Borders: %d\n", settings->ui.border_theme);

    free_config(settings);
}

// int main() {
//     test_file_loading();
// }