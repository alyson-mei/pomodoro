#include "literals.h"
#include "ui.h"

const Border minimal_top_border     = {" -", "-", "- "};
const Border minimal_mid_border     = {"|",  " ",  "|"};
const Border minimal_midb_border    = {"|",  "-",  "|"};
const Border minimal_bot_border     = {" -", "-", "- "};

const BoxBorders minimal_borders = {
    .top        = &minimal_top_border,
    .mid        = &minimal_mid_border,
    .mid_bottom = &minimal_midb_border,
    .bottom     = &minimal_bot_border
};

const Border single_top_border     = {"┌", "─", "┐"};
const Border single_mid_border     = {"│", " ", "│"};
const Border single_midb_border    = {"├", "─", "┤"};
const Border single_bot_border     = {"└", "─", "┘"};

const BoxBorders single_borders = {
    .top        = &single_top_border,
    .mid        = &single_mid_border,
    .mid_bottom = &single_midb_border,
    .bottom     = &single_bot_border
};

const Border double_top_border     = {"╔", "═", "╗"};
const Border double_mid_border     = {"║", " ", "║"};
const Border double_midb_border    = {"╠", "═", "╣"};
const Border double_bot_border     = {"╚", "═", "╝"};

const BoxBorders double_borders = {
    .top        = &double_top_border,
    .mid        = &double_top_border,
    .mid_bottom = &double_midb_border,
    .bottom     = &double_bot_border
};