#define SDL_MAIN_HANDLED
#include <locale.h>
#include <ncurses/curses.h>
#include "../include/menu.h"
#include "../include/game.h"
#include "../include/audio.h"

int main() {
    // NCURSES
    setlocale(LC_ALL, "\\");
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    // AUDIO
    if (!initAudio()) {
        endwin();
        return 1;
    }

    playMenuBGM();

    bool jalan = true;
    while (jalan) {
        int pilih = showMenu();

        if (pilih == 0) {
            stopBGM();      // stop menu music
            jalankanGame(); // game → playBGM()
            playMenuBGM();  // balik ke menu
        } else {
            jalan = false;
        }
    }

    closeAudio();
    endwin();
    return 0;
}
