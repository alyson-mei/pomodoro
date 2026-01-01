#ifndef COMMAND_H

typedef enum {
    CMD_NONE,
    CMD_TOGGLE_PAUSE,
    CMD_QUIT,
    CMD_CONTINUE
} Command;

Command read_command(void);

#endif