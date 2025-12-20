#include <stdio.h>
#include <string.h>

#define CLR_RESET "\x1b[0m"

void print_gradient(const char *s,
                    int r1, int g1, int b1,
                    int r2, int g2, int b2)
{
    size_t len = strlen(s);

    for (size_t i = 0; i < len; i++) {
        float t = (len <= 1) ? 0.0f : (float)i / (len - 1);

        int r = r1 + t * (r2 - r1);
        int g = g1 + t * (g2 - g1);
        int b = b1 + t * (b2 - b1);

        printf("\x1b[38;2;%d;%d;%dm%c", r, g, b, s[i]);
    }

    printf(CLR_RESET "\n");
}

int main() {
    print_gradient("GRADIENT TEXT",
                   255, 0, 0,     // red
                   0, 120, 255);  // blue
    return 0;
}
