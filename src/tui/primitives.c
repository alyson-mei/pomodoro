#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../include/timer.h"
#include "../../include/ui/ui.h"
#include "../../include/global.h"
#include "../../include/literals.h"


// Helper
char* repeat_string(char *buf, const char *ch, int count) {
    size_t ch_len = strlen(ch);  // Cache this!
    for (int i = 0; i < count; i++) {
        memcpy(buf, ch, ch_len);  // memcpy faster than strcpy
        buf += ch_len;
    }
    *buf = '\0';  // Null terminate
    return buf;
}

void box_line_to_buf(
    char *buf,
    const char *str,
    const Border *border,
    int width
) {
    char *p = buf;
    int slen = strlen(str);

    int total_pad = width - slen;
    int left_pad  = total_pad / 2;
    int right_pad = total_pad - left_pad;

    p = repeat_string(p, border->left_char, 1);
    p = repeat_string(p, border->mid_char, left_pad);

    memcpy(p, str, slen);
    p += slen;

    p = repeat_string(p, border->mid_char, right_pad);
    p = repeat_string(p, border->right_char, 1);

    *p = '\0';
}

void box_render_line(
    const char *str,
    const Border *border,
    int width,
    const char* color,
    int padding_horizontal,
    int paint_content
) {
    bool special_case = strcmp(border->left_char, "-") == 0 && 
                       strcmp(border->right_char, "-") == 0;

    if (!special_case)
        printf("%s%s%s", color, border->left_char, RESET_COLOR);
    else 
        printf(" ");
    
    int str_len = strlen(str);
    const char *display_str = str;
    char truncated[COMMON_STR_SIZE];
    
    if (str_len > width) {
        int max_len = width - padding_horizontal;
        if (max_len < 0) max_len = 0;
        snprintf(truncated, sizeof(truncated), "%.*s...", max_len, str);
        display_str = truncated;
        str_len = strlen(truncated);  // Only call strlen once on truncated
    }
    
    int left_pad = (width - str_len) / 2;
    int right_pad = width - str_len - left_pad;
    
    if (paint_content)
        printf("%s", color);
    
    // Cache mid_char length
    size_t mid_len = strlen(border->mid_char);
    for (int i = 0; i < left_pad; i++) {
        fwrite(border->mid_char, 1, mid_len, stdout);  // Faster than printf
    }
    
    printf("%s", display_str);
    
    for (int i = 0; i < right_pad; i++) {
        fwrite(border->mid_char, 1, mid_len, stdout);
    }
    
    if (paint_content)
        printf("%s", RESET_COLOR);
    
    if (!special_case)
        printf("%s%s%s\n", color, border->right_char, RESET_COLOR);
    else 
        printf(" \n");
}