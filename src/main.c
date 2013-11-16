
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ncurses.h>

#include "signal.h"

void sigHandler (va_list args)
{
    int key = va_arg(args, int);
    printw("Key: %d\n", key);
}

int main(int argc, char **argv)
{
    Signal sig;
    int c;

    setlocale(LC_CTYPE, "");

    initscr();
    raw();
    keypad(stdscr, 1);
    noecho();
    start_color();
    use_default_colors();

    init_pair(1, -1, -1);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);

    memset(&sig, 0, sizeof(Signal));

    signalConnect (&sig, sigHandler);

    while ((c = getch()) != '\n') {
        if (c > 0)
            signalEmit (&sig, c);
    }

    endwin();
    
    return 0;
}

