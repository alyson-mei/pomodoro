#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../include/timer.h"
#include "../../include/display.h"
#include "../../include/global.h"


const char* ui_color_code(UiColor c) {
    switch (c) {
        case UI_COLOR_RED:           return "\x1b[31m";
        case UI_COLOR_GREEN:         return "\x1b[32m";
        case UI_COLOR_YELLOW:        return "\x1b[33m";
        case UI_COLOR_BLUE:          return "\x1b[34m";
        case UI_COLOR_MAGENTA:       return "\x1b[35m";
        case UI_COLOR_CYAN:          return "\x1b[36m";
        case UI_COLOR_GRAY:          return "\x1b[90m";
        
        // Gentler cyberpunk colors (using 256-color mode)
        case UI_COLOR_SOFT_CYAN:     return "\x1b[38;5;80m";   // Soft cyan-blue
        case UI_COLOR_SOFT_PURPLE:   return "\x1b[38;5;141m";  // Soft purple/lavender
        case UI_COLOR_SOFT_RED:      return "\x1b[38;5;203m";  // Soft coral red
        case UI_COLOR_SOFT_GREEN:    return "\x1b[38;5;114m";  // Soft mint green
        case UI_COLOR_NEON_PINK:     return "\x1b[38;5;213m";  // Neon pink
        case UI_COLOR_ELECTRIC_BLUE: return "\x1b[38;5;81m";   // Electric blue
        
        case UI_COLOR_DEFAULT:
        default:                     return "\x1b[0m";
    }
}
