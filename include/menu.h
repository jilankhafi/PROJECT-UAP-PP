#include <ncurses/curses.h>
#include "../include/menu.h"
#include "../include/game.h"

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    while (true) {
        clear();
        int pilihan = tampilkanMenu();

        if (pilihan == 0) {
            startGame();
        } 
        else if (pilihan == 1) {
            break;
        }
    }

    endwin();
    return 0;
}
