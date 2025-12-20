#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <locale.h>

#define CLR_RESET "\x1b[0m"
#define HIDE_CURSOR "\x1b[?25l"
#define SHOW_CURSOR "\x1b[?25h"
#define CLEAR "\033[2J\033[H\033[?25l"
#define HOME "\x1b[H"

// Set RGB foreground color
static void set_rgb(int r, int g, int b) {
    printf("\x1b[38;2;%d;%d;%dm", r, g, b);
}

// Draw a single frame of animated cyberpunk box
void draw_box(const char *text, int width, int height, float t) {
    size_t len = strlen(text);
    int pad = (width - (int)len) / 2;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width + 2; col++) { // +2 for borders
            float f = (float)col / (width + 1);

            // Neon gradient color
            int r = (int)(255 * (0.5 + 0.5 * sin(t + f * 3.14)));
            int g = (int)(50 + 205 * f);
            int b = (int)(255 * (0.5 + 0.5 * cos(t + f * 3.14)));
            set_rgb(r, g, b);

            if (row == 0) { // top border
                if (col == 0) printf("╔");
                else if (col == width + 1) printf("╗");
                else printf("═");
            } else if (row == height - 1) { // bottom border
                if (col == 0) printf("╚");
                else if (col == width + 1) printf("╝");
                else printf("═");
            } else { // middle rows
                if (col == 0 || col == width + 1) printf("║");
                else if (col - 1 >= pad && col - 1 < pad + len)
                    printf("%c", text[col - 1 - pad]);
                else
                    printf(" ");
            }
        }
        printf(CLR_RESET "\n"); // end of row
    }
}

int main() {
    setlocale(LC_ALL, "");  // enable UTF-8
    printf(HIDE_CURSOR);
    printf(CLEAR);

    const char *text = "TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT TEXT";
    int height = 30;
    int width = 60;
    float t = 0.0f;

    while (1) {
        printf(CLEAR);
        fflush(stdout);
        printf(HOME);       // move cursor to top-left
        
        draw_box(text, width, height, t);
        t += 0.1f;          // animate
        usleep(33000);      // ~30 FPS
    }

    printf(SHOW_CURSOR);
    return 0;
}
