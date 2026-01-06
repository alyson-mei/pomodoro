#ifndef COMMAND_H

#include <stdbool.h>

typedef enum {
    KCMD_NONE,
    KCMD_TOGGLE_PAUSE,
    KCMD_QUIT,
    KCMD_CONTINUE,
    KCMD_MESSAGE
} KeyCommand;

typedef enum {
    TCMD_DEFAULT,
    TCMD_POMODORO,
    TCMD_STOPWATCH,
    TCMD_EXPORT,
    TCMD_HELP,
    TCMD_INVALID
} CliCommand;

typedef struct {
    CliCommand command;

    int work_minutes;
    int break_minutes;
    int cycles;

    const char *category;
    const char *activity;
} CliArgs;

bool parse_cli(int argc, char **argv, CliArgs *out);

KeyCommand read_command(void);

#endif