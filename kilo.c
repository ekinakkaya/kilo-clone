#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>


struct termios original_termios;


void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}


void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_raw_mode);

    struct termios raw = original_termios;
    /* &= (bitwise-AND)
     * ~  (bitwise-OR)
     * ECHO : 00000000000000000000000000001000
     * ~ECHO: 11111111111111111111111111110111
     * original_termios.c_lflag & ~ECHO will be the same except the 4th bit
     * will be zero.
     */
    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}



int main() {
    enable_raw_mode();


    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if (iscntrl(c)) {
            printf("%d\n", c);
        } else {
            printf("%d ('%c')\n", c, c);
        }
    }


    return 0;
}
