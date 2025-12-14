#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../include/audio.h"

static Mix_Music* bgm = nullptr;
static Mix_Chunk* shootSFX = nullptr;
static Mix_Chunk* explodeSFX = nullptr;
static Mix_Chunk* gameOverSFX = nullptr;
static Mix_Chunk* waveSFX = nullptr;
static Mix_Music* menuBGM = nullptr;


bool initAudio()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL AUDIO ERROR: %s\n", SDL_GetError());
        return false;
    }

    int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    if ((Mix_Init(flags) & flags) != flags) {
        printf("MIX INIT ERROR: %s\n", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("MIX OPEN ERROR: %s\n", Mix_GetError());
        return false;
    }

    Mix_AllocateChannels(16);

    menuBGM = Mix_LoadMUS("audio/menu.wav");
    bgm     = Mix_LoadMUS("audio/bgm.wav");
    shootSFX    = Mix_LoadWAV("audio/shoot.wav");
    explodeSFX  = Mix_LoadWAV("audio/explode.wav");
    gameOverSFX = Mix_LoadWAV("audio/gameover.wav");
    waveSFX     = Mix_LoadWAV("audio/wave.wav");

    if (!menuBGM)    printf("LOAD menu.wav ERROR: %s\n", Mix_GetError());
    if (!bgm)        printf("LOAD bgm.wav ERROR: %s\n", Mix_GetError());
    if (!shootSFX)   printf("LOAD shoot.wav ERROR: %s\n", Mix_GetError());
    if (!explodeSFX) printf("LOAD explode.wav ERROR: %s\n", Mix_GetError());
    if (!gameOverSFX)printf("LOAD gameover.wav ERROR: %s\n", Mix_GetError());
    if (!waveSFX)    printf("LOAD wave.wav ERROR: %s\n", Mix_GetError());

    if (!menuBGM || !bgm || !shootSFX || !explodeSFX || !gameOverSFX || !waveSFX)
        return false;

    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_Volume(-1, MIX_MAX_VOLUME / 2);

    return true;
}

void playBGM()
{
    if (bgm && !Mix_PlayingMusic())
        Mix_PlayMusic(bgm, -1);
}

void playMenuBGM() {
    if (menuBGM) {
        Mix_HaltMusic();
        Mix_PlayMusic(menuBGM, -1);
    }
}

void stopBGM()
{
    Mix_HaltMusic();
}

void sfxShoot()
{
    Mix_PlayChannel(-1, shootSFX, 0);
}

void sfxExplosion()
{
    Mix_PlayChannel(-1, explodeSFX, 0);
}

void sfxGameOver()
{
    Mix_PlayChannel(-1, gameOverSFX, 0);
}

void sfxWave()
{
    Mix_PlayChannel(-1, waveSFX, 0);
}

void closeAudio()
{
    Mix_FreeChunk(shootSFX);
    Mix_FreeChunk(explodeSFX);
    Mix_FreeChunk(gameOverSFX);
    Mix_FreeMusic(bgm);
    Mix_FreeMusic(menuBGM);

    Mix_CloseAudio();
    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
