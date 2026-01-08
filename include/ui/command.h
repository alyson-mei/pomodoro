#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

// Keyboard commands during timer execution
typedef enum {
    KCMD_NONE,
    KCMD_TOGGLE_PAUSE,
    KCMD_QUIT,
    KCMD_CONTINUE,
    KCMD_MESSAGE
} KeyCommand;

// CLI commands - no parameters
typedef enum {
    TCMD_COUNTDOWN,
    TCMD_STOPWATCH,
    TCMD_EXPORT,
    TCMD_INVALID
} CliCommand;

CliCommand parse_cli_command(int argc, char **argv);
KeyCommand read_command(void);

#endif