#include <stdio.h>

#include "../../include/timer.h"
#include "../../include/display.h"


void draw_progress_bar(int percent, int width) {
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int filled = (percent * width) / 100;

    for (int i = 0; i < width; ++i) {
        if (i < filled)
            printf("█");
        else
            printf("░");
    }
}

int calculate_progress(const Timer *t) {
    if (t->target_ms <= 0) return 100;

    int64_t elapsed = get_elapsed_ms(t);

    if (elapsed <= 0) return 0;
    if (elapsed >= t->target_ms) return 100;

    return (int)((elapsed * 100) / t->target_ms);
}


void pomodoro_render(const Timer *t) {
    DisplayTime td = get_time_display(t);
    
    // Clear screen and hide cursor
    printf("\033[2J\033[H\033[?25l");
    fflush(stdout);
        
    // Top border
    printf("\n");
    printf("┌─────────────────────────────────┐\n");
    printf("│  🍅  POMODORO TIMER             │\n");
    printf("├─────────────────────────────────┤\n");
    
    // Time (centered, large)
    printf("│                                 │\n");
    printf("│          %02d:%02d:%02d               │\n", 
           td.minutes, td.seconds, td.centiseconds / 10);
    
    // Progress bar
    int progress = calculate_progress(t);  // 0-100
    printf("│     ");
    draw_progress_bar(progress, 18);
    printf("  %3d%%    │\n", progress);
    
    // Empty line
    printf("│                                 │\n");
    
    // Category info
    printf("│     %s → %s", t->category, t->subcategory);
    // Pad to align with box width
    printf("      │\n");
    
    // Controls
    printf("│                                 │\n");
    printf("│   [Space] Pause  [Q] Quit       │\n");

    printf("│                                 │\n");
    printf("└─────────────────────────────────┘\n");
    
    fflush(stdout);
}