#ifndef GLOBAL_H
#define GLOBAL_H


#include <termios.h>

#define BUF_SIZE_XXS       16
#define BUF_SIZE_XS        32
#define BUF_SIZE_S         64
#define BUF_SIZE_M         128
#define BUF_SIZE_L         256
 


extern struct termios old_tio;


enum {
    COMMON_STR_SIZE    = BUF_SIZE_L,
    MODE_SIZE          = BUF_SIZE_XXS,

    STR_CONFIG_SIZE    = BUF_SIZE_L,
    KEY_CONFIG_SIZE    = BUF_SIZE_XS,  
    
    CATEGORY_SIZE      = BUF_SIZE_M,
    ACTIVITY_SIZE      = BUF_SIZE_M,
    CAT_ACT_SIZE       = BUF_SIZE_M * 2 + BUF_SIZE_XXS,
    COMMENT_SIZE       = BUF_SIZE_L, 

    HEADER_SIZE        = BUF_SIZE_S,
    TIME_SIZE          = BUF_SIZE_XS,
    PROGRESS_BAR_SIZE  = BUF_SIZE_M,
    CONTROLS_SIZE      = BUF_SIZE_XS,
    BUF_PERCENT_SIZE   = BUF_SIZE_XXS,
    COLOR_SIZE         = BUF_SIZE_XS

};

#endif