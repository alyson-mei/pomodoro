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

static CliCommand parse_command(const char *s) {
    if (!s)                 return TCMD_DEFAULT;
    if (!strcmp(s, "pomodoro"))  return TCMD_POMODORO;
    if (!strcmp(s, "stopwatch")) return TCMD_STOPWATCH;
    if (!strcmp(s, "export"))    return TCMD_EXPORT;
    if (!strcmp(s, "help"))      return TCMD_HELP;
    return TCMD_INVALID;
}


bool parse_cli(int argc, char **argv, CliArgs *args) {
    memset(args, 0, sizeof(*args));

    args->command = parse_command(argc > 1 ? argv[1] : NULL);

    if (args->command == TCMD_INVALID)
        return false;

    for (int i = 2; i < argc; i++) {

        if (!strcmp(argv[i], "--cycles")) {
            if (i + 1 >= argc) return false;
            args->cycles = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--work")) {
            if (i + 1 >= argc) return false;
            args->work_minutes = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--break")) {
            if (i + 1 >= argc) return false;
            args->break_minutes = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "--category")) {
            if (i + 1 >= argc) return false;
            args->category = argv[++i];
        }
        else if (!strcmp(argv[i], "--activity")) {
            if (i + 1 >= argc) return false;
            args->activity = argv[++i];
        }
        else {
            return false;  // unknown flag
        }
    }

    return true;
}