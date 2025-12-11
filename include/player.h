#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <memory>

class Bullet;
class TextureManager;
class Game;

class Player {
public:
    Player(Game* game, SDL_Renderer* renderer, int x, int y);
    ~Player();

    void handleInput(const Uint8* keyboardState, float dt);
    void update(float dt);
    void render(SDL_Renderer* renderer);
    void shoot();

    SDL_Rect getBounds() const;
    int getX() const;
    int getY() const;

private:
    Game* m_game;
    SDL_Texture* m_texture;
    SDL_Rect m_dest;
    float m_speed; // pixels per second
    float m_shotCooldown;
    float m_shotTimer;
    SDL_Renderer* m_renderer;
};

#endif // PLAYER_H
