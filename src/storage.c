#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define BUF_SIZE 256
#define MAX_CATEGORIES 64
#define MAX_SUBCATEGORIES 64

#define DAYS_IN_A_MOTHS 30.44 // approx
#define PROGRESS_YAML_PATH "../progress.yaml"
#define PROGRESS_BIN_PATH "../data/progress.bin"

typedef struct {
    char name[BUF_SIZE];
    int64_t total_ms;
} Subcategory;

typedef struct {
    char name[BUF_SIZE];
    Subcategory subs[MAX_SUBCATEGORIES];
    size_t count;
    int64_t total_ms;
} Category;

typedef struct internal
{
    Category categories[MAX_CATEGORIES];
    size_t count;
    int64_t total_ms;
} CategoryList;

char *format_duration_ms(int64_t ms, char *buf, size_t bufsize, int truncate_zero) {
    int64_t total_seconds = ms / 1000;
    int seconds = total_seconds % 60;

    int64_t total_minutes = total_seconds / 60;
    int minutes = total_minutes % 60;

    int64_t total_hours = total_minutes / 60;
    int hours = total_hours % 24;

    int64_t total_days = total_hours / 24;
    int days = total_days % 30;

    int64_t total_months = total_days / 30.44;   
    int months = total_months % 12;

    int years  = total_months / 12;

    // Format string, skipping zero units if requested
    size_t len = 0;

    if (!truncate_zero || years)  len += snprintf(buf + len, bufsize - len, "%dy ", years);
    if (!truncate_zero || months) len += snprintf(buf + len, bufsize - len, "%dmo ", months);
    if (!truncate_zero || days)   len += snprintf(buf + len, bufsize - len, "%dd ", days);
    if (!truncate_zero || hours)  len += snprintf(buf + len, bufsize - len, "%dh ", hours);
    if (!truncate_zero || minutes)len += snprintf(buf + len, bufsize - len, "%dm ", minutes);
    if (!truncate_zero || seconds)len += snprintf(buf + len, bufsize - len, "%ds", seconds);

    // Remove trailing space
    if (len > 0 && buf[len-1] == ' ') buf[len-1] = '\0';

    return buf;
}

void add_time(
    CategoryList *list,
    const char *category_name,
    const char *sub_name,
    int64_t ms) {
    size_t i, j;

    // 1) Find or create category
    for (i = 0; i < list->count; i++) {
        if (strncmp(list->categories[i].name, category_name, BUF_SIZE) == 0)
            break;
    }

    Category *cat;
    if (i < list->count) {
        cat = &list->categories[i];
    } else {
        // new category
        if (list->count >= MAX_CATEGORIES) return; // max categories reached
        cat = &list->categories[list->count++];
        strncpy(cat->name, category_name, BUF_SIZE-1);
        cat->name[BUF_SIZE-1] = '\0';
        cat->count = 0;
        cat->total_ms = 0;
    }

    // 2) Find or create subcategory
    for (j = 0; j < cat->count; j++) {
        if (strncmp(cat->subs[j].name, sub_name, BUF_SIZE) == 0)
            break;
    }

    Subcategory *sub;
    if (j < cat->count) {
        sub = &cat->subs[j];
    } else {
        // new subcategory
        if (cat->count >= MAX_SUBCATEGORIES) return; // max subcategories reached
        sub = &cat->subs[cat->count++];
        strncpy(sub->name, sub_name, BUF_SIZE-1);
        sub->name[BUF_SIZE-1] = '\0';
        sub->total_ms = 0;
    }

    // 3) Update total
    list->total_ms += ms;
    cat->total_ms += ms;
    sub->total_ms += ms;
}

int save_categories(const CategoryList *list, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;

    // Write the main structure
    if (fwrite(list, sizeof(CategoryList), 1, f) != 1) {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}

int load_categories(CategoryList *list, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    if (fread(list, sizeof(CategoryList), 1, f) != 1) {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}

void write_yaml(const CategoryList *list, const char *path) {
    char buf[BUF_SIZE];  // buffer for formatted durations

    FILE *f = fopen(path, "w");
    if (!f) return;

    for (size_t i = 0; i < list->count; i++) {
        const Category *cat = &list->categories[i];
        format_duration_ms(cat->total_ms, buf, BUF_SIZE, 1);
        fprintf(f, "%s:\n    total: %s\n", cat->name, buf);

        for (size_t j = 0; j < cat->count; j++) {
            const Subcategory *sub = &cat->subs[j];
            format_duration_ms(sub->total_ms, buf, BUF_SIZE, 1);
            fprintf(f, "    - %s: %s\n", sub->name, buf);
        }
    }

    fclose(f);
}

int main() {
    CategoryList list = {0}; // initialize empty list
    
    if (!load_categories(&list, PROGRESS_BIN_PATH)) {
        printf("No previous data found, starting fresh.\n");
    }

    // Add some times
    add_time(&list, "Work", "Coding", 3600*1000);       // 1 hour
    add_time(&list, "Work", "Emails", 1800*1000);       // 30 min
    add_time(&list, "Hobby", "Painting", 7200*1000);    // 2 hours
    add_time(&list, "Work", "Coding", 5400*1000);       // add 1.5 hours more


    if (!save_categories(&list, PROGRESS_BIN_PATH)) {
        printf("Error saving categories!\n");
    }

    // Write YAML file
    write_yaml(&list, PROGRESS_YAML_PATH);

    return 0;
}

