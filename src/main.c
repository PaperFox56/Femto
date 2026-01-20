#include <unistd.h> // read the standard input
#include <stdlib.h>

#include <termios.h>

// Save of the original terminal state to be rrestored at the end of the program
struct termios original_termios_flags;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios_flags);
}

void enable_raw_mode() {
    struct termios termios_flags;

    // get the current terminal attributes
    tcgetattr(STDIN_FILENO, &termios_flags);
    original_termios_flags = termios_flags; // save for latter

    // make sure the original state of the program is restored at exit
    atexit(disable_raw_mode);

    termios_flags.c_lflag &= ~(ECHO); // turn off the ECHO feature

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_flags);
}
int main() {

    enable_raw_mode();

    char c;

    // read the standard input
    while (read(STDIN_FILENO, &c, 1) == 1 && c!= 'q');

    return 0;
}