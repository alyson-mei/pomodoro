#include <termios.h> 
#include <unistd.h> 
#include <fcntl.h>

struct termios old_tio;


// Setup non-blocking input

void setup_terminal(struct termios *old_tio) {
    struct termios new_tio;
    
    tcgetattr(STDIN_FILENO, old_tio);
    new_tio = *old_tio;
    
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void restore_terminal(struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}
