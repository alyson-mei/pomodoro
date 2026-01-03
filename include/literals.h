#ifndef LITERALS_H
#define LITERALS_H

#define TIMER_COUNTDOWN   "COUNTDOWN"
#define TIMER_STOPWATCH   "STOPWATCH"

#define TIMER_WORK       "WORK"
#define TIMER_BREAK      "BREAK"
#define TIMER_LONG_BREAK "LONG BREAK"

#define TIMER_PAUSE      "PAUSE"
#define TIMER_CANCELLED   "CANCELLED"

#define CONTROLS_ACTIVE     "[Space] Pause    [Q] Quit"
#define CONTROLS_PAUSED     "[Space] Resume   [Q] Quit"
#define CONTROLS_COMPLETED  "[Enter] Continue [Q] Quit" 
#define CONTROLS_CANCELLED  "     [Enter/Q]   Quit    "


// Layout themes

#define LT_MINIMAL    "MINIMAL"
#define LT_SMALL      "SMALL"  
#define LT_STANDARD   "STANDARD"

// Color themes

#define CT_MINIMAL    "MINIMAl"
#define CT_SIMPLE     "SIMPLE"
#define CT_CYBERPUNK  "CYBERPUNK"
#define CT_FOREST     "FOREST"
#define CT_SUNSET     "SUNSET"

// Border types

#define BT_MINIMAL    "MINIMAL"
#define BT_SINGLE     "SINGLE"
#define BT_DOUBLE     "DOUBLE" 

// Progress bar themes
#define PT_MINIMAL    "MINIMAL"
#define PT_SIMPLE     "SIMPLE"
#define PT_STANDARD   "STANDARD"

// Colors

#define RED           "\x1b[31m"
#define GREEN         "\x1b[32m"
#define YELLOW        "\x1b[33m"
#define BLUE          "\x1b[34m"
#define MAGENTA       "\x1b[35m"
#define CYAN          "\x1b[36m"
#define GRAY          "\x1b[90m"

#define SOFT_CYAN     "\x1b[38;5;80m"
#define SOFT_PURPLE   "\x1b[38;5;141m"
#define SOFT_RED      "\x1b[38;5;203m"
#define SOFT_GREEN    "\x1b[38;5;114m"
#define NEON_PINK     "\x1b[38;5;213m"
#define ELECTRIC_BLUE "\x1b[38;5;81m"

#define RESET_COLOR   "\x1b[0m"

#define UI_COLOR_RESET "\x1b[0m"


#endif
