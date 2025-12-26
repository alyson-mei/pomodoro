#ifndef GLOBAL_H
#define GLOBAL_H

#define BUF_SIZE_XXS       16
#define BUF_SIZE_XS        32
#define BUF_SIZE_S         64
#define BUF_SIZE_M         128
#define BUF_SIZE_L         256
 
enum {
    STR_CONFIG_MAX_SIZE    = BUF_SIZE_L,
    KEY_CONFIG_MAX_SIZE    = BUF_SIZE_XS,  
    CATEGORY_MAX_SIZE      = BUF_SIZE_M,
    ACTIVITY_MAX_SIZE      = BUF_SIZE_L
};

#endif