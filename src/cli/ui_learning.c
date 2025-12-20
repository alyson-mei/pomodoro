#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include <locale.h>

#include "../../include/timer.h"
#include "../../include/display.h"

typedef struct {
    const char *left_char;
    const char *mid_char;
    const char *right_char;
} Border;

typedef struct 
{
    const char *header;
    const char *time;
    const char *controls;
} BoxContent;

static char* repeat_char(char *buf, const char *ch, int count) {
    for (int i = 0; i < count; i++) {
        strcpy(buf, ch);
        buf += strlen(ch); 
    }
    return buf;
}

void box_string(char *buf, const char *str, Border *border, int width) {
    char *p = buf;
    int slen = strlen(str);

    int total_pad = width - slen;
    int left_pad  = total_pad / 2;
    int right_pad = total_pad - left_pad;

    p = repeat_char(p, border->left_char, 1);
    p = repeat_char(p, border->mid_char, left_pad);

    memcpy(p, str, slen);
    p += slen;

    p = repeat_char(p, border->mid_char, right_pad);
    p = repeat_char(p, border->right_char, 1);

    *p = '\0';
}


void box_with_content(const char *str, Border *top, Border *mid, Border *bottom) {
    char buf[128];


}

void box_render(BoxContent *content) {
    Border top = {"╔", "═", "╗"};
    Border mid = {"║", " ", "║"};
    Border bottom = {"╚", "═", "╝"};
    Border mid_bottom = {"╠", "═", "╣"};

    char buf[128];

    box_string(buf, "", &top, 40);
    printf("%s\n", buf);
    box_string(buf, content->header, &mid, 40);
    printf("%s\n", buf);
    box_string(buf, "", &mid_bottom, 40);
    printf("%s\n", buf);

    box_string(buf, "", &mid, 40);
    printf("%s\n", buf);
    box_string(buf, content->time, &mid, 40);
    printf("%s\n", buf);
    box_string(buf, "", &mid, 40);
    printf("%s\n", buf);

    box_string(buf, content->controls, &mid, 40);
    printf("%s\n", buf);
    box_string(buf, "", &mid, 40);
    printf("%s\n", buf);
    box_string(buf, "", &bottom, 40);
    printf("%s\n", buf);

}


int main() {
    setlocale(LC_ALL, "");
    char buf[128];

    Border top = {"╔", "═", "╗"};
    Border mid = {"║", " ", "║"};
    Border bottom = {"╚", "═", "╝"};

    box_string(buf, "", &top, 40);
    printf("%s\n", buf);

    box_string(buf, "123", &mid, 40);
    for (int i = 0; i < 5; i++) {
        printf("%s\n", buf);
    }

    box_string(buf, "", &bottom, 40);
    printf("%s\n\n", buf);


    BoxContent content = {
        "POMODORO TIMER",
        "10:20:30",
        "[Space] Pause  [Q] Quit"
    };
    box_render(&content);
    return 0;
}
