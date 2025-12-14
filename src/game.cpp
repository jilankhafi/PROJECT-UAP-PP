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

/* =====================
   INPUT
===================== */
void inputPemain()
{
    int tombol = getch();

    switch (tombol)
    {
    case 'a':
    case KEY_LEFT:
        if (pemainX > 1)
            pemainX--;
        break;

    case 'd':
    case KEY_RIGHT:
        if (pemainX < lebarLayar - 2)
            pemainX++;
        break;

    case ' ':
        if (cooldownTembakPemain == 0)
        {
            for (int i = 0; i < MAKS_PELURU; i++)
            {
                if (!peluruPemain[i].aktif)
                {
                    peluruPemain[i] = {pemainX, pemainY - 1, true};
                    cooldownTembakPemain = MAKS_COOLDOWN_TEMBAK;
                    sfxShoot();
                    break;
                }
            }
        }
        break;

    case 'q':
    case 'Q':
        gameBerjalan = false;
        break;
    }
}

/* =====================
   UPDATE
===================== */
void updateCooldown()
{
    if (cooldownTembakPemain > 0)
        cooldownTembakPemain--;
}

void updatePeluru()
{
    for (int i = 0; i < MAKS_PELURU; i++)
    {
        if (peluruPemain[i].aktif)
        {
            peluruPemain[i].y--;
            if (peluruPemain[i].y <= 0)
                peluruPemain[i].aktif = false;
        }

        if (peluruMusuh[i].aktif)
        {
            peluruMusuh[i].y++;
            if (peluruMusuh[i].y >= tinggiLayar - 1)
                peluruMusuh[i].aktif = false;
        }
    }
}

void updateMusuh()
{
    for (int i = 0; i < jumlahMusuh; i++)
    {
        if (!musuh[i].hidup)
            continue;

        musuh[i].counterGerak++;
        if (musuh[i].counterGerak >= DELAY_GERAK_MUSUH)
        {
            musuh[i].x += musuh[i].arah;

            if (musuh[i].x <= 1 || musuh[i].x >= lebarLayar - 2)
                musuh[i].arah *= -1;

            musuh[i].counterGerak = 0;
        }

        if (rand() % 120 == 0)
        {
            for (int j = 0; j < MAKS_PELURU; j++)
            {
                if (!peluruMusuh[j].aktif)
                {
                    peluruMusuh[j] = {musuh[i].x, musuh[i].y + 1, true};
                    break;
                }
            }
        }
    }
}

/* =====================
   COLLISION
===================== */
void cekTabrakan()
{
    for (int i = 0; i < MAKS_PELURU; i++)
    {
        if (!peluruPemain[i].aktif)
            continue;

        for (int j = 0; j < jumlahMusuh; j++)
        {
            if (musuh[j].hidup &&
                peluruPemain[i].x == musuh[j].x &&
                peluruPemain[i].y == musuh[j].y)
            {

                musuh[j].hidup = false;
                peluruPemain[i].aktif = false;
                skor += 10;
                sfxExplosion();
            }
        }
    }

    for (int i = 0; i < MAKS_PELURU; i++)
    {
        if (peluruMusuh[i].aktif &&
            peluruMusuh[i].x == pemainX &&
            peluruMusuh[i].y == pemainY)
        {
            sfxGameOver();
            gameBerjalan = false;
        }
    }
}

/* =====================
   WAVE
===================== */
void cekWave()
{
    bool semuaMati = true;

    for (int i = 0; i < jumlahMusuh; i++)
    {
        if (musuh[i].hidup)
        {
            semuaMati = false;
            break;
        }
    }

    if (semuaMati)
    {
        wave++;
        timerGame = WAKTU_PER_WAVE;
        sfxWave();

        jumlahMusuh += 2;
        if (jumlahMusuh > MAKS_MUSUH)
            jumlahMusuh = MAKS_MUSUH;

        for (int i = 0; i < jumlahMusuh; i++)
        {
            musuh[i] = {
                2 + rand() % (lebarLayar - 4),
                2 + rand() % 3,
                true,
                (rand() % 2 == 0 ? -1 : 1),
                0};
        }
    }
}

/* =====================
   RENDER
===================== */
void renderGame()
{
    clear();
    box(stdscr, 0, 0);

    mvprintw(pemainY, pemainX, "^");

    for (int i = 0; i < jumlahMusuh; i++)
        if (musuh[i].hidup)
            mvprintw(musuh[i].y, musuh[i].x, "V");

    for (int i = 0; i < MAKS_PELURU; i++)
    {
        if (peluruPemain[i].aktif)
            mvprintw(peluruPemain[i].y, peluruPemain[i].x, "|");

        if (peluruMusuh[i].aktif)
            mvprintw(peluruMusuh[i].y, peluruMusuh[i].x, "!");
    }

    mvprintw(1, 2, "Skor: %d", skor);
    mvprintw(1, lebarLayar / 2 - 6, "Wave: %d", wave);
    mvprintw(1, lebarLayar - 15, "Waktu: %d", timerGame / 20);

    refresh();
}

/* =====================
   GAME LOOP
===================== */
void jalankanGame()
{
    inisialisasiGame();
    playBGM();

    while (gameBerjalan)
    {
        inputPemain();
        updateCooldown();
        updatePeluru();
        updateMusuh();
        cekTabrakan();
        cekWave();
        renderGame();

        timerGame--;
        if (timerGame <= 0)
        {
            sfxGameOver();
            gameBerjalan = false;
        }

        napms(50);
    }

    stopBGM();

    if (skor > highscore)
    {
        ofstream file(FILE_HIGHSCORE);
        file << skor;
        file.close();
        highscore = skor;
    }

    // ===== GAME OVER =====
    nodelay(stdscr, FALSE);
    clear();
    box(stdscr, 0, 0);

    mvprintw(tinggiLayar / 2 - 1, lebarLayar / 2 - 5, "GAME OVER");
    mvprintw(tinggiLayar / 2, lebarLayar / 2 - 8, "Skor     : %d", skor);
    mvprintw(tinggiLayar / 2 + 1, lebarLayar / 2 - 8, "Highscore: %d", highscore);
    mvprintw(tinggiLayar / 2 + 3, lebarLayar / 2 - 14, "Tekan Q untuk kembali ke menu");

    refresh();

    int ch;
    while ((ch = getch()) != 'q' && ch != 'Q')
    {
    }

    clear();
    refresh();
    nodelay(stdscr, TRUE);

