#include <ncurses/curses.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "../include/game.h"
#include "../include/audio.h"

using namespace std;

/* =====================
   KONSTANTA
===================== */
#define MAKS_MUSUH 20
#define MAKS_PELURU 50

#define DELAY_GERAK_MUSUH 10
#define MAKS_COOLDOWN_TEMBAK 8

#define WAKTU_PER_WAVE (60 * 20)
#define FILE_HIGHSCORE "data/highscore.txt"

/* =====================
   STRUKTUR DATA
===================== */
struct Peluru
{
    int x, y;
    bool aktif;
};

struct Musuh
{
    int x, y;
    bool hidup;
    int arah;
    int counterGerak;
};

/* =====================
   VARIABEL GLOBAL
===================== */
int lebarLayar, tinggiLayar;

int pemainX, pemainY;
int skor = 0;
int highscore = 0;

int wave = 1;
int timerGame = WAKTU_PER_WAVE;
int cooldownTembakPemain = 0;

bool gameBerjalan = true;
int jumlahMusuh = 6;

Peluru peluruPemain[MAKS_PELURU];
Peluru peluruMusuh[MAKS_PELURU];
Musuh musuh[MAKS_MUSUH];

/* =====================
   UTIL
===================== */
void resetPeluru(Peluru arr[], int jumlah)
{
    for (int i = 0; i < jumlah; i++)
        arr[i].aktif = false;
}

/* =====================
   INIT GAME
===================== */
void inisialisasiGame()
{
    getmaxyx(stdscr, tinggiLayar, lebarLayar);

    skor = 0;
    wave = 1;
    timerGame = WAKTU_PER_WAVE;
    gameBerjalan = true;
    jumlahMusuh = 6;
    cooldownTembakPemain = 0;

    pemainX = lebarLayar / 2;
    pemainY = tinggiLayar - 2;

    srand(time(nullptr));

    resetPeluru(peluruPemain, MAKS_PELURU);
    resetPeluru(peluruMusuh, MAKS_PELURU);

    for (int i = 0; i < jumlahMusuh; i++)
    {
        musuh[i] = {
            2 + rand() % (lebarLayar - 4),
            2 + rand() % 3,
            true,
            (rand() % 2 == 0 ? -1 : 1),
            0};
    }

    ifstream file(FILE_HIGHSCORE);
    if (file.is_open())
    {
        file >> highscore;
        file.close();
    }
}
