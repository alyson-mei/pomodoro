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

// CLI commands
typedef enum {
    TCMD_COUNTDOWN,
    TCMD_STOPWATCH,
    TCMD_EXPORT,
    TCMD_STATS,
    TCMD_INVALID
} CliCommand;

typedef enum {
    EXPORT_CSV,
    EXPORT_YAML
} ExportFormat;

typedef struct {
    CliCommand command;
    ExportFormat format;
} CliArgs;

bool parse_cli_command(int argc, char **argv, CliArgs *args);
KeyCommand read_command(void);

#endif