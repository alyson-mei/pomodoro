#include "literals.h"
#include "ui.h"

static const Border minimal_borders[4] = {
    {"-", "-", "-"},
    {"|", " ", "|"},
    {"|", "-", "|"},
    {"-", "-", "-"}
};

static const Border single_borders[4] = {
    {"┌", "─", "┐"},
    {"│", " ", "│"},
    {"├", "─", "┤"},
    {"└", "─", "┘"}
};

static const Border double_borders[4] = {
    {"╔", "═", "╗"},
    {"║", " ", "║"},
    {"╠", "═", "╣"},
    {"╚", "═", "╝"}
};

const BoxBorders* get_borders(BorderType type) {
    static const BoxBorders border_sets[] = {
        [BORDER_MINIMAL] = {
            .top = &minimal_borders[0],
            .mid = &minimal_borders[1],
            .mid_bottom = &minimal_borders[2],
            .bottom = &minimal_borders[3]
        },
        [BORDER_SINGLE] = {
            .top = &single_borders[0],
            .mid = &single_borders[1],
            .mid_bottom = &single_borders[2],
            .bottom = &single_borders[3]
        },
        [BORDER_DOUBLE] = {
            .top = &double_borders[0],
            .mid = &double_borders[1],
            .mid_bottom = &double_borders[2],
            .bottom = &double_borders[3]
        }
    };
    return &border_sets[type];
}

const Colors* get_colors(ColorTheme theme) {
    static const Colors colors[] = {
        [THEME_MINIMAL] = {
            .active    = RESET_COLOR,
            .paused    = RESET_COLOR,
            .completed = RESET_COLOR,
            .cancelled = RESET_COLOR
        },
        [THEME_SIMPLE] = {
            .active    = GREEN,
            .paused    = YELLOW,
            .completed = BLUE,
            .cancelled = GRAY
        },
        [THEME_CYBERPUNK] = {
            .active    = NEON_PINK,
            .paused    = ELECTRIC_BLUE,
            .completed = SOFT_CYAN,
            .cancelled = SOFT_PURPLE
        },
        [THEME_FOREST] = {
            .active    = SOFT_GREEN,
            .paused    = YELLOW,
            .completed = CYAN,
            .cancelled = GRAY
        },
        [THEME_SUNSET] = {
            .active    = SOFT_RED,
            .paused    = YELLOW,
            .completed = MAGENTA,
            .cancelled = GRAY
        }
    };
    return &colors[theme];
}
