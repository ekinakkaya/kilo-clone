#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>


#define CTRL_KEY(k) ((k) & 0x1f)


struct termios original_termios;


void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

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


char editor_read_key() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1) != 1)) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}



void editor_process_keypress() {
    char c = editor_read_key();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);

            exit(0);
            break;
    }
}



void editor_refresh_screen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}


int main() {
    enable_raw_mode();


    while (1) {
        editor_refresh_screen();
        editor_process_keypress();
    }


    return 0;
}

