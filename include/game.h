#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <memory>
#include <vector>

class Player;
class Enemy;
class Bullet;

class Game {
public:
    Game();
    ~Game();

    // Initialize SDL subsystems, window and renderer
    bool init(const char* title, int width, int height, bool fullscreen = false);

    // Load assets (textures, fonts)
    bool loadAssets(const char* fontPath);

    // Game loop
    void run();

    // Clean up everything
    void clean();

    // Getter for renderer (used by other modules)
    SDL_Renderer* getRenderer() const;

    // Screen size
    int getWidth() const;
    int getHeight() const;

    // Methods used by objects
    void spawnEnemy();
    void spawnEnemyAt(int x, int y);

    // Manage bullets/enemies
    void addBullet(std::unique_ptr<Bullet> b);
    void addEnemy(std::unique_ptr<Enemy> e);

private:
    void processInput(float dt);
    void update(float dt);
    void render();
    void handleCollisions();

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;

    int m_width;
    int m_height;
    bool m_isRunning;

    // Game objects
    std::unique_ptr<Player> m_player;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Bullet>> m_bullets;

    // Timing for spawning enemies
    float m_enemySpawnTimer;
    float m_enemySpawnInterval;

    // Score & lives
    int m_score;
    int m_lives;
};

#endif // GAME_H
