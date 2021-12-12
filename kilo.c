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

    /* disable
     * IXON     Ctrl-s and Ctrl-q
     * ICRNL    carriage return new line
     */
    raw.c_iflag &= ~(ICRNL | IXON);


    /* disable
     * OPOST    output processing (such as the newline \r\n thing)
     */
    raw.c_oflag &= ~(OPOST);


    /* disable
     * ECHO     echo
     * ICANON   canonical mode
     * ISIG     Ctrl-c and Ctrl-v signals
     * IEXTEN   Ctrl-v
     *
     */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);


    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}



int main() {
    enable_raw_mode();


    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
    }


    return 0;
}
