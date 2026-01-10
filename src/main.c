#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "../include/global.h"
#include "../include/timer.h"
#include "../include/config.h"
#include "../include/literals.h"
#include "../include/data.h"
#include "../include/session.h"
#include "../include/stats.h" 

#include "../include/ui/ui.h"
#include "../include/ui/command.h"
#include "../include/ui/setup.h"

#define TEMP_FILE "data/.temp_entry"


int main(int argc, char **argv) {
    
    ensure_data_dir();
    check_crashed_session();

    Settings *settings = load_config("./config.ini");
    if (!settings) {
        printf("Failed to load config\n");
        return 1;
    }

    CliArgs args;
    if (!parse_cli_command(argc, argv, &args)) {
        if (argc == 1) {
            // No arguments - run interactive countdown
            run_timer_session(settings, MODE_COUNTDOWN);
            free_config(settings);
            return 0;
        } else {
            // Invalid command - show error and exit
            fprintf(stderr, "Invalid command. Usage: cpomo [countdown|stopwatch|export --history|export --stats]\n");
            free_config(settings);
            return 1;
        }
    }

    int result = 0;
    
    switch (args.command) {
        case TCMD_EXPORT_HISTORY:
            if (!export_entries_csv("data/entries.dat", "history.csv")) {
                fprintf(stderr, "Export failed\n");
                result = 1;
            } else {
                printf("Exported history to history.csv\n");
            }
            break;
        
        case TCMD_EXPORT_STATS: {
            StatsData stats;
            if (!build_stats_from_entries("data/entries.dat", &stats)) {
                fprintf(stderr, "Failed to build stats\n");
                result = 1;
                break;
            }
            
            if (!export_stats_yaml(&stats, "stats.yaml")) {
                fprintf(stderr, "Failed to export stats\n");
                result = 1;
            } else {
                printf("Exported stats to stats.yaml\n");
            }
            break;
        }
        
        case TCMD_STOPWATCH:
            run_timer_session(settings, MODE_STOPWATCH);
            break;
            
        case TCMD_COUNTDOWN:
            run_timer_session(settings, MODE_COUNTDOWN);
            break;
            
        default:
            result = 1;
    }
    
    free_config(settings);
    return result;
}