#include <ncurses/curses.h>
#include "../include/menu.h"

int tampilkanMenu() {
    const char *opsi[] = { "Start Game", "Exit" };
    int jumlahOpsi = 2;
    int pilihan = 0;

    int tinggi = 6, lebar = 20;
    int startY = 10, startX = 30;

    WINDOW *menu_win = newwin(tinggi, lebar, startY, startX);
    keypad(menu_win, TRUE);

    while (true) {
        werase(menu_win);
        box(menu_win, 0, 0);

        for (int i = 0; i < jumlahOpsi; i++) {
            if (i == pilihan) wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, i + 1, 2, opsi[i]);
            wattroff(menu_win, A_REVERSE);
        }

        wrefresh(menu_win);
        int key = wgetch(menu_win);

        if (key == KEY_UP)
            pilihan = (pilihan == 0) ? jumlahOpsi - 1 : pilihan - 1;
        else if (key == KEY_DOWN)
            pilihan = (pilihan == jumlahOpsi - 1) ? 0 : pilihan + 1;
        else if (key == 10)
            break;
    }

    delwin(menu_win);
    return pilihan;
}
