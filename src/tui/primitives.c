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
    for (int i = 0; i < count; i++) {
        strcpy(buf, ch);
        buf += strlen(ch); 
    }
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
    int paint_content         // 0 = don't color content, 1 = color content
) {
    
    bool special_case = strcmp(border->left_char, "-") == 0 && strcmp(border->right_char, "-") == 0;

    // Print left border in color
    if (!special_case)
    printf("%s%s%s", color, border->left_char, UI_COLOR_RESET);
    else printf(" ");
    
    // Truncate string if too long
    char truncated[COMMON_STR_SIZE];
    int str_len = strlen(str);
    
    if (str_len > width) {
        // Truncate and add "..."
        int max_len = width - padding_horizontal; // Leave room for "..."
        if (max_len < 0) max_len = 0;
        snprintf(truncated, sizeof(truncated), "%.*s...", max_len, str);
        str = truncated;
        str_len = strlen(str);
    }
    
    // Calculate padding
    int left_pad = (width - str_len) / 2;
    int right_pad = width - str_len - left_pad;
    
    // Print middle (with or without color)
    if (paint_content) {
        printf("%s", color);
    }
    
    for (int i = 0; i < left_pad; i++) printf("%s", border->mid_char);
    printf("%s", str);
    for (int i = 0; i < right_pad; i++) printf("%s", border->mid_char);
    
    if (paint_content) {
        printf("%s", UI_COLOR_RESET);
    }
    
    // Print right border in color
    if (!special_case)
    printf("%s%s%s\n", color, border->right_char, UI_COLOR_RESET);
    else printf(" \n");
}
