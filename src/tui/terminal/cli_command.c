#include <string.h>
#include <stdio.h>
#include "../../../include/ui/command.h"

bool parse_cli_command(int argc, char **argv, CliArgs *args) {
    memset(args, 0, sizeof(*args));
    args->format = EXPORT_CSV; // default
    
    // No arguments = invalid (not interactive mode)
    if (argc < 2) {
        args->command = TCMD_INVALID;
        return false;
    }

    const char *cmd = argv[1];
    
    if (!strcmp(cmd, "countdown")) {
        args->command = TCMD_COUNTDOWN;
    }
    else if (!strcmp(cmd, "stopwatch")) {
        args->command = TCMD_STOPWATCH;
    }
    else if (!strcmp(cmd, "export")) {
        args->command = TCMD_EXPORT;
    }
    else if (!strcmp(cmd, "stats")) {
        args->command = TCMD_STATS;
    }
    else {
        args->command = TCMD_INVALID;
        return false;
    }

    // Parse format flag for export/stats commands
    if (args->command == TCMD_EXPORT || args->command == TCMD_STATS) {
        if (argc >= 3) {
            if (!strcmp(argv[2], "--yaml") || !strcmp(argv[2], "-y")) {
                args->format = EXPORT_YAML;
            }
            else if (!strcmp(argv[2], "--csv") || !strcmp(argv[2], "-c")) {
                args->format = EXPORT_CSV;
            }
            else {
                fprintf(stderr, "Unknown format: %s (use --yaml or --csv)\n", argv[2]);
                return false;
            }
        }
    }

    return true;
}