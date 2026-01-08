#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "../../../include/ui/command.h"

KeyCommand read_command(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return KCMD_NONE;
    }

    switch (c) {
        case ' ':
            return KCMD_TOGGLE_PAUSE;
        case 'q':
        case 'Q':
            return KCMD_QUIT;
        case 'm':
        case 'M':
            return KCMD_MESSAGE;
        case '\n':
        case '\r':
            return KCMD_CONTINUE;
        default:
            return KCMD_NONE;
    }
}
