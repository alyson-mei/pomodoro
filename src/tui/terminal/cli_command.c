#include <string.h>
#include <stdio.h>
#include "../../../include/ui/command.h"

CliCommand parse_cli_command(int argc, char **argv) {
    // cpomo <command>
    // Need exactly 2 arguments: program name + command
    if (argc != 2) {
        return TCMD_INVALID;
    }

    const char *cmd = argv[1];
    
    if (!strcmp(cmd, "countdown"))  return TCMD_COUNTDOWN;
    if (!strcmp(cmd, "stopwatch"))  return TCMD_STOPWATCH;
    if (!strcmp(cmd, "export"))     return TCMD_EXPORT;
    
    return TCMD_INVALID;
}

