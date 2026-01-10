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
        // No command or invalid - run interactive countdown
        run_timer_session(settings, MODE_COUNTDOWN);
        free_config(settings);
        return 0;
    }

    int result = 0;
    
    switch (args.command) {
        case TCMD_EXPORT: {
            const char *output = args.format == EXPORT_YAML ? "history.csv" : "history.csv";
            if (!export_entries_csv("data/entries.dat", output)) {
                fprintf(stderr, "Export failed\n");
                result = 1;
            } else {
                printf("Exported entries to %s\n", output);
            }
            break;
        }
        
        case TCMD_STATS: {
            StatsData stats;
            if (!build_stats_from_entries("data/entries.dat", &stats)) {
                fprintf(stderr, "Failed to build stats\n");
                result = 1;
                break;
            }
            
            const char *output = args.format == EXPORT_YAML ? "stats.yaml" : "stats.csv";
            bool success = args.format == EXPORT_YAML ? 
                export_stats_yaml(&stats, output) : 
                export_stats_csv(&stats, output);
            
            if (!success) {
                fprintf(stderr, "Failed to export stats\n");
                result = 1;
            } else {
                printf("Exported stats to %s\n", output);
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
