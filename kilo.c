#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>


struct termios original_termios;


void die(const char *s) {
    perror(s);
    exit(1);
}


void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1)
        die("tcsetattr");
}


void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1)
        die("tcgetattr");

    atexit(disable_raw_mode);

    struct termios raw = original_termios;

    /* disable
     * IXON     Ctrl-s and Ctrl-q
     * ICRNL    carriage return new line
     * BRKINT
     * INPCK
     * ISTRIP
     */
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON);


    /* disable
     * OPOST    output processing (such as the newline \r\n thing)
     */
    raw.c_oflag &= ~(OPOST);


    raw.c_lflag |= (CS8);


    /* disable
     * ECHO     echo
     * ICANON   canonical mode
     * ISIG     Ctrl-c and Ctrl-v signals
     * IEXTEN   Ctrl-v
     */
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);


    /* minimum number of bytes required for read() to return. */
    raw.c_cc[VMIN] = 0;

    /* max amount of time to wait before read() returns. */
    raw.c_cc[VTIME] = 1;


    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}



int main() {
    enable_raw_mode();


    while (1) {
        char c = '\0';


        if (read(STDIN_FILENO, &c, 1) == -1 && errno == EAGAIN)
            die("read");
        
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        
        if (c == 'q') break;
    }


    return 0;
}

