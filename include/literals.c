#include "literals.h"
#include "ui/ui.h"

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

static const ProgressBar minimal_bar = {
    .left_char = "",
    .mid_char_off = ".",
    .mid_char_on = "#",
    .right_char = "",
    .char_width = 1  // ← ASCII characters
};

static const ProgressBar simple_bar = {
    .left_char = "[",
    .mid_char_off = " ",
    .mid_char_on = "=",
    .right_char = "]",
    .char_width = 1  // ← ASCII characters
};

static const ProgressBar standard_bar = {
    .left_char = "",
    .mid_char_off = "░",
    .mid_char_on = "█",
    .right_char = "",
    .char_width = 2  // ← UTF-8 blocks need double width
};

const BoxBorders* get_borders(BorderTheme type) {
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

const ProgressBar* get_progress_bar(ProgressTheme theme) {
    switch (theme) {
        case PB_THEME_MINIMAL:     return &minimal_bar;
        case PB_THEME_SIMPLE:      return &simple_bar;
        case PB_THEME_STANDARD:    return &standard_bar;
        default:                return &minimal_bar;
    }
}