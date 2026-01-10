#include <string.h>
#include <stdio.h>
#include "../../../include/ui/command.h"

bool parse_cli_command(int argc, char **argv, CliArgs *args) {
    memset(args, 0, sizeof(*args));
    
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
        // Need second argument: --history or --stats
        if (argc < 3) {
            fprintf(stderr, "export requires --history or --stats\n");
            args->command = TCMD_INVALID;
            return false;
        }
        
        if (!strcmp(argv[2], "--history")) {
            args->command = TCMD_EXPORT_HISTORY;
        }
        else if (!strcmp(argv[2], "--stats")) {
            args->command = TCMD_EXPORT_STATS;
        }
        else {
            fprintf(stderr, "Unknown export type: %s (use --history or --stats)\n", argv[2]);
            args->command = TCMD_INVALID;
            return false;
        }
    }
    else {
        args->command = TCMD_INVALID;
        return false;
    }

    return true;
}