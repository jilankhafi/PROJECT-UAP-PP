#include <ncurses/curses.h>
#include <fstream>
#include <string>
#include "../include/menu.h"

using namespace std;

static const char* menuItems[] = {
    "> Start Game",
    "$ Highscore",
    "? Help",
    "! Exit"
};
static const int MENU_COUNT = 4;

void drawTitle(int startY, int startX) {
    const char* title[] = {
        "  /$$$$$$  /$$   /$$ /$$$$$$$$ /$$$$$$$   /$$$$$$ ",
        " /$$__  $$| $$  | $$| $$_____/| $$__  $$ /$$__  $$",
        "| $$  \\__/| $$  | $$| $$      | $$  \\ $$| $$  \\ $$",
        "|  $$$$$$ | $$$$$$$$| $$$$$   | $$$$$$$/| $$  | $$",
        " \\____  $$| $$__  $$| $$__/   | $$__  $$| $$  | $$",
        " /$$  \\ $$| $$  | $$| $$      | $$  \\ $$| $$  | $$",
        "|  $$$$$$/| $$  | $$| $$$$$$$$| $$  | $$|  $$$$$$/",
        " \\______/ |__/  |__/|________/|__/  |__/ \\______/ "
    };

    for (int i = 0; i < 8; i++) {
        mvprintw(startY + i, startX, title[i]);
    }
}

void showHighscore() {
    clear();
    box(stdscr, 0, 0);

    int highscore = 0;
    ifstream file("data/highscore.txt");
    if (file.is_open()) {
        file >> highscore;
        file.close();
    }

    mvprintw(5, 5, "$ HIGHSCORE");
    mvprintw(7, 5, "Skor Tertinggi : %d", highscore);
    mvprintw(10, 5, "Tekan tombol apa saja untuk kembali");

    refresh();
    getch();
}

void showHelp() {
    clear();
    box(stdscr, 0, 0);

    mvprintw(4, 4, "? HELP / CONTROLS");
    mvprintw(6, 4, "A / <   : Gerak ke kiri");
    mvprintw(7, 4, "D / >   : Gerak ke kanan");
    mvprintw(8, 4, "SPACE   : Tembak");
    mvprintw(9, 4, "Q       : Keluar dari game");

    mvprintw(12, 4, "Tekan tombol apa saja untuk kembali ke menu");

    refresh();
    getch();
}

int showMenu() {
    int pilihan = 0;
    int ch;

    nodelay(stdscr, FALSE);

    while (true) {
        clear();
        box(stdscr, 0, 0);

        int h, w;
        getmaxyx(stdscr, h, w);

        // ===== TITLE =====
        int titleWidth = 58;
        int titleX = (w - titleWidth) / 2;
        drawTitle(2, titleX);

        // ===== MENU BOX =====
        int boxWidth = 28;
        int boxHeight = MENU_COUNT * 2 + 2;
        int boxStartY = 12;
        int boxStartX = (w - boxWidth) / 2;

        WINDOW* menuWin = newwin(boxHeight, boxWidth, boxStartY, boxStartX);
        box(menuWin, 0, 0);

        for (int i = 0; i < MENU_COUNT; i++) {
            if (i == pilihan) wattron(menuWin, A_REVERSE);
            mvwprintw(menuWin, 1 + i * 2, 3, menuItems[i]);
            if (i == pilihan) wattroff(menuWin, A_REVERSE);
        }

        wrefresh(menuWin);
        mvprintw(h - 2, 2, "Gunakan ^ v  Enter | ESC untuk keluar");
        refresh();

        ch = getch();
        delwin(menuWin);

        switch (ch) {
        case KEY_UP:
            pilihan = (pilihan - 1 + MENU_COUNT) % MENU_COUNT;
            break;
        case KEY_DOWN:
            pilihan = (pilihan + 1) % MENU_COUNT;
            break;
        case '\n':
            if (pilihan == 0) { nodelay(stdscr, TRUE); return 0; }
            if (pilihan == 1) showHighscore();
            if (pilihan == 2) showHelp();
            if (pilihan == 3) { nodelay(stdscr, TRUE); return 1; }
            break;
        case 27:
            nodelay(stdscr, TRUE);
            return 1;
        }
    }
}
