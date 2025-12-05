#include <ncurses/curses.h>
#include "../include/game.h"

void startGame() {
    clear();
    nodelay(stdscr, TRUE); // input tidak menunggu
    keypad(stdscr, TRUE);

    int ch;

    while (true) {
        clear();
        mvprintw(5, 10, "GAME SEDANG BERJALAN");
        mvprintw(7, 10, "Tekan Q untuk kembali ke menu");
        refresh();

        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
    }

    nodelay(stdscr, FALSE); // kembalikan ke mode normal
}
