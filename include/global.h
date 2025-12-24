#ifndef GLOBAL_H
#define GLOBAL_H

#define BUF_SIZE_XXS 16
#define BUF_SIZE_XS  32
#define BUF_SIZE_S   64
#define BUF_SIZE_M   128
#define BUF_SIZE_L   256

enum {
    MAX_PRESETS = BUF_SIZE_XS,
    MAX_CYCLES_PER_PRESET = BUF_SIZE_XS
};

#endif
