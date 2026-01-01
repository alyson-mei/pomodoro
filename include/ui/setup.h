#ifndef SETUP_H
#define SETUP_H

void setup_terminal(struct termios *old_tio);
void restore_terminal(struct termios *old_tio);

#endif