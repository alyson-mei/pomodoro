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

    CliCommand cmd = parse_cli_command(argc, argv);
    int result = 0;
    
    switch (cmd) {
        case TCMD_EXPORT:
            if (!export_entries_csv("data/entries.dat", "history.csv")) {
                fprintf(stderr, "Export failed\n");
                result = 1;
            } else {
                printf("Exported to history.csv\n");
            }
            break;
            
        case TCMD_STOPWATCH:
            run_timer_session(settings, MODE_STOPWATCH);
            break;
            
        case TCMD_COUNTDOWN:
        case TCMD_INVALID:  // Default behavior when no command given
            run_timer_session(settings, MODE_COUNTDOWN);
            break;
    }
    
    free_config(settings);
    return result;
}