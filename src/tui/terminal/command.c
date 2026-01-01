#include <unistd.h>
#include <fcntl.h>
#include "../../../include/ui/command.h"

Command read_command(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return CMD_NONE;
    }

    switch (c) {
        case ' ':
            return CMD_TOGGLE_PAUSE;
        case 'q':
        case 'Q':
            return CMD_QUIT;
        case '\n':
        case '\r':
            return CMD_CONTINUE;
        default:
            return CMD_NONE;
    }
}