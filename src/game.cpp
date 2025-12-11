#include <ncurses/ncurses.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <windows.h>

using namespace std;

// Konstanta game
const int GAME_WIDTH = 80;
const int GAME_HEIGHT = 24;
const int MAX_ENEMIES = 12; // Kurangi maksimal musuh
const int MAX_STARS = 100;
const int MAX_POWERUPS = 5;
const int MAX_BULLETS = 100;

// Tipe peluru
enum BulletType {
    NORMAL,
    LASER,
    MISSILE,
    SPREAD,
    PIERCE
};

// Tipe alien/musuh
enum AlienType {
    BASIC_ALIEN,
    FAST_ALIEN,
    STRONG_ALIEN,
    SHOOTER_ALIEN,
    BOSS_ALIEN,
    MINION_ALIEN
};

// Struktur untuk posisi
struct Position {
    int x, y;
};

// Kelas untuk sprite besar (untuk boss dan musuh besar)
class BigSprite {
private:
    vector<string> sprite;
    Position pos;
    int width, height;
    
public:
    BigSprite(int x, int y, const vector<string>& spriteData) : pos{x, y}, sprite(spriteData) {
        height = sprite.size();
        if (height > 0) width = sprite[0].length();
    }
    
    void setPosition(int x, int y) { pos.x = x; pos.y = y; }
    Position getPosition() const { return pos; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void draw(int colorPair) {
        attron(COLOR_PAIR(colorPair));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (sprite[i][j] != ' ') {
                    mvaddch(pos.y + i, pos.x + j, sprite[i][j]);
                }
            }
        }
        attroff(COLOR_PAIR(colorPair));
    }
    
    bool collidesWith(int x, int y) {
        return (x >= pos.x && x < pos.x + width && 
                y >= pos.y && y < pos.y + height &&
                sprite[y - pos.y][x - pos.x] != ' ');
    }
};

// Kelas dasar untuk semua objek game
class GameObject {
protected:
    Position pos;
    char symbol;
    int colorPair;
    
public:
    GameObject(int x, int y, char s, int c) : symbol(s), colorPair(c) {
        pos.x = x;
        pos.y = y;
    }
    
    virtual ~GameObject() {}
    
    virtual void draw() {
        attron(COLOR_PAIR(colorPair));
        mvaddch(pos.y, pos.x, symbol);
        attroff(COLOR_PAIR(colorPair));
    }
    
    virtual void update() {}
    
    Position getPosition() const { return pos; }
    void setPosition(int x, int y) { pos.x = x; pos.y = y; }
    char getSymbol() const { return symbol; }
    int getColor() const { return colorPair; }
};

// Kelas untuk pemain
class Player : public GameObject {
private:
    int lives;
    int score;
    int powerLevel;
    int powerTimer;
    bool isPowered;
    int weaponType;
    int weaponCooldown;
    
public:
    Player(int x, int y) : GameObject(x, y, '^', 1), lives(5), score(0), 
                          powerLevel(0), powerTimer(0), isPowered(false), 
                          weaponType(NORMAL), weaponCooldown(0) {}
    
    void moveLeft() {
        if (pos.x > 1) pos.x--;
    }
    
    void moveRight() {
        if (pos.x < GAME_WIDTH - 3) pos.x++;
    }
    
    void moveUp() {
        if (pos.y > 1) pos.y--;
    }
    
    void moveDown() {
        if (pos.y < GAME_HEIGHT - 3) pos.y++;
    }
    
    void addScore(int points) {
        score += points;
    }
    
    void loseLife() {
        if (!isPowered) {
            lives--;
        }
    }
    
    void gainLife() {
        if (lives < 10) lives++;
    }
    
    void activatePower(int duration, int weapon = NORMAL) {
        isPowered = true;
        powerTimer = duration;
        weaponType = weapon;
        
        switch(weapon) {
            case LASER: symbol = 'L'; break;
            case MISSILE: symbol = 'M'; break;
            case SPREAD: symbol = 'S'; break;
            case PIERCE: symbol = 'P'; break;
            default: symbol = '^'; break;
        }
    }
    
    void updatePower() {
        if (isPowered) {
            powerTimer--;
            if (powerTimer <= 0) {
                isPowered = false;
                weaponType = NORMAL;
                symbol = '^';
            }
        }
        
        if (weaponCooldown > 0) weaponCooldown--;
    }
    
    bool canShoot() {
        int cooldown = 10;
        switch(weaponType) {
            case LASER: cooldown = 5; break;
            case MISSILE: cooldown = 20; break;
            case SPREAD: cooldown = 15; break;
            case PIERCE: cooldown = 12; break;
        }
        
        return weaponCooldown <= 0;
    }
    
    void resetCooldown() {
        switch(weaponType) {
            case LASER: weaponCooldown = 5; break;
            case MISSILE: weaponCooldown = 20; break;
            case SPREAD: weaponCooldown = 15; break;
            case PIERCE: weaponCooldown = 12; break;
            default: weaponCooldown = 10; break;
        }
    }
    
    int getLives() const { return lives; }
    int getScore() const { return score; }
    int getPowerLevel() const { return powerLevel; }
    bool getPowered() const { return isPowered; }
    int getWeaponType() const { return weaponType; }
    
    void draw() override {
        if (isPowered) {
            attron(A_BOLD);
            switch(weaponType) {
                case LASER: attron(COLOR_PAIR(12)); break;
                case MISSILE: attron(COLOR_PAIR(13)); break;
                case SPREAD: attron(COLOR_PAIR(14)); break;
                case PIERCE: attron(COLOR_PAIR(15)); break;
                default: attron(COLOR_PAIR(6)); break;
            }
        } else {
            attron(COLOR_PAIR(colorPair));
        }
        
        // Buat pemain lebih besar
        mvaddch(pos.y, pos.x, '^');
        mvaddch(pos.y + 1, pos.x - 1, '<');
        mvaddch(pos.y + 1, pos.x, 'A');
        mvaddch(pos.y + 1, pos.x + 1, '>');
        
        if (isPowered) {
            attroff(A_BOLD);
            switch(weaponType) {
                case LASER: attroff(COLOR_PAIR(12)); break;
                case MISSILE: attroff(COLOR_PAIR(13)); break;
                case SPREAD: attroff(COLOR_PAIR(14)); break;
                case PIERCE: attroff(COLOR_PAIR(15)); break;
                default: attroff(COLOR_PAIR(6)); break;
            }
        } else {
            attroff(COLOR_PAIR(colorPair));
        }
    }
    
    // Untuk deteksi tabrakan dengan sprite besar
    bool collidesWith(int x, int y) {
        return (x == pos.x && y == pos.y) ||
               (x == pos.x - 1 && y == pos.y + 1) ||
               (x == pos.x && y == pos.y + 1) ||
               (x == pos.x + 1 && y == pos.y + 1);
    }
};

// Kelas untuk peluru
class Bullet : public GameObject {
private:
    int direction;
    int speed;
    BulletType type;
    int damage;
    int pierceCount;
    
public:
    Bullet(int x, int y, int dir, BulletType t = NORMAL, int s = 1) 
        : GameObject(x, y, getSymbol(t), getColor(t)), direction(dir), speed(s), type(t) {
        damage = getDamage(t);
        pierceCount = (t == PIERCE) ? 3 : 1;
    }
    
    void update() override {
        pos.y += direction * speed;
    }
    
    bool isOutOfBounds() const {
        return pos.y <= 0 || pos.y >= GAME_HEIGHT - 1;
    }
    
    int getDirection() const { return direction; }
    BulletType getBulletType() const { return type; }
    int getDamage() const { return damage; }
    
    bool pierce() {
        if (type == PIERCE && pierceCount > 0) {
            pierceCount--;
            return true;
        }
        return false;
    }
    
    static char getSymbol(BulletType t) {
        switch(t) {
            case LASER: return '|';
            case MISSILE: return '!';
            case SPREAD: return '/';
            case PIERCE: return '>';
            default: return '|';
        }
    }
    
    static int getColor(BulletType t) {
        switch(t) {
            case LASER: return 12;
            case MISSILE: return 13;
            case SPREAD: return 14;
            case PIERCE: return 15;
            default: return 2;
        }
    }
    
    static int getDamage(BulletType t) {
        switch(t) {
            case LASER: return 2;
            case MISSILE: return 5;
            case SPREAD: return 1;
            case PIERCE: return 3;
            default: return 1;
        }
    }
    
    void draw() override {
        if (direction == -1) {
            attron(A_BOLD);
            attron(COLOR_PAIR(getColor(type)));
        } else {
            attron(COLOR_PAIR(3));
        }
        
        mvaddch(pos.y, pos.x, symbol);
        
        if (direction == -1) {
            attroff(A_BOLD);
            attroff(COLOR_PAIR(getColor(type)));
        } else {
            attroff(COLOR_PAIR(3));
        }
    }
};

// Sprite untuk boss besar
vector<string> BOSS_SPRITE = {
    " ▄▄▄▄▄ ",
    "▐█████▌",
    "▐█▀▀▀█▌",
    "▐█▄ ▄█▌",
    " ▀▀▀▀▀ "
};

// Sprite untuk musuh besar
vector<string> BIG_ENEMY_SPRITE = {
    " ▄▄▄ ",
    "▐███▌",
    "▐▀▄▀▌",
    " ▀▀▀ "
};

// Sprite untuk penembak/shooter
vector<string> SHOOTER_SPRITE = {
    " █▀▀█ ",
    "▐█ █▌",
    " ▀▄▀ "
};

// Kelas untuk alien/musuh
class Alien : public GameObject {
private:
    AlienType type;
    int health;
    int maxHealth;
    int movePattern;
    int moveTimer;
    int shootCooldown;
    int points;
    int shootPattern;
    BigSprite* bigSprite;
    
public:
    Alien(int x, int y, AlienType t) : GameObject(x, y, getSymbol(t), getColor(t)), 
                                      type(t), moveTimer(0), shootCooldown(rand() % 100 + 100) {
        switch(type) {
            case BASIC_ALIEN:
                health = maxHealth = 3;
                points = 50;
                bigSprite = new BigSprite(x-2, y-1, BIG_ENEMY_SPRITE);
                break;
            case FAST_ALIEN:
                health = maxHealth = 2;
                points = 75;
                break;
            case STRONG_ALIEN:
                health = maxHealth = 8;
                points = 150;
                bigSprite = new BigSprite(x-2, y-1, BIG_ENEMY_SPRITE);
                break;
            case SHOOTER_ALIEN:
                health = maxHealth = 5;
                points = 100;
                bigSprite = new BigSprite(x-2, y-1, SHOOTER_SPRITE);
                break;
            case BOSS_ALIEN:
                health = maxHealth = 100;
                points = 1000;
                bigSprite = new BigSprite(x-3, y-2, BOSS_SPRITE);
                break;
            case MINION_ALIEN:
                health = maxHealth = 1;
                points = 25;
                break;
        }
        movePattern = rand() % 4;
    }
    
    ~Alien() {
        if (bigSprite) delete bigSprite;
    }
    
    void update() override {
        // Pergerakan alien
        moveTimer++;
        
        switch(type) {
            case BASIC_ALIEN:
                if (moveTimer % 2 == 0) pos.y++;
                break;
            case FAST_ALIEN:
                pos.y++;
                break;
            case STRONG_ALIEN:
                if (moveTimer % 3 == 0) pos.y++;
                break;
            case SHOOTER_ALIEN:
                if (moveTimer % 2 == 0) pos.y++;
                if (moveTimer % 40 < 20) {
                    if (pos.x > 3) pos.x--;
                } else {
                    if (pos.x < GAME_WIDTH - 4) pos.x++;
                }
                break;
            case BOSS_ALIEN:
                if (moveTimer % 80 < 40) {
                    if (pos.x > GAME_WIDTH/4) pos.x--;
                } else {
                    if (pos.x < 3*GAME_WIDTH/4) pos.x++;
                }
                break;
            case MINION_ALIEN:
                pos.y++;
                if (moveTimer % 20 < 10) {
                    if (pos.x > 2) pos.x--;
                } else {
                    if (pos.x < GAME_WIDTH - 3) pos.x++;
                }
                break;
        }
        
        // Update sprite besar
        if (bigSprite) {
            switch(type) {
                case BASIC_ALIEN:
                case STRONG_ALIEN:
                    bigSprite->setPosition(pos.x-2, pos.y-1);
                    break;
                case SHOOTER_ALIEN:
                    bigSprite->setPosition(pos.x-2, pos.y-1);
                    break;
                case BOSS_ALIEN:
                    bigSprite->setPosition(pos.x-3, pos.y-2);
                    break;
            }
        }
        
        // Cooldown tembakan
        if (shootCooldown > 0) shootCooldown--;
    }
    
    bool isOutOfBounds() const {
        return pos.y >= GAME_HEIGHT || pos.x <= 0 || pos.x >= GAME_WIDTH;
    }
    
    bool takeDamage(int damage) {
        health -= damage;
        return health <= 0;
    }
    
    bool canShoot() {
        int chance = 0;
        switch(type) {
            case SHOOTER_ALIEN: chance = 80; break;
            case BOSS_ALIEN: chance = 50; break;
            case BASIC_ALIEN: chance = 10; break;
            case STRONG_ALIEN: chance = 15; break;
            default: chance = 5; break;
        }
        
        if (shootCooldown <= 0 && rand() % 100 < chance) {
            shootCooldown = getShootCooldown();
            return true;
        }
        return false;
    }
    
    int getShootCooldown() {
        switch(type) {
            case SHOOTER_ALIEN: return 60;
            case BOSS_ALIEN: return 40;
            default: return 120;
        }
    }
    
    vector<Bullet*> shoot() {
        vector<Bullet*> bullets;
        
        switch(type) {
            case SHOOTER_ALIEN:
                bullets.push_back(new Bullet(pos.x, pos.y + 2, 1, NORMAL, 1));
                break;
            case BOSS_ALIEN:
                // Tembakan spread dari boss
                bullets.push_back(new Bullet(pos.x, pos.y + 3, 1, NORMAL, 1));
                bullets.push_back(new Bullet(pos.x - 2, pos.y + 3, 1, NORMAL, 1));
                bullets.push_back(new Bullet(pos.x + 2, pos.y + 3, 1, NORMAL, 1));
                bullets.push_back(new Bullet(pos.x - 4, pos.y + 2, 1, NORMAL, 1));
                bullets.push_back(new Bullet(pos.x + 4, pos.y + 2, 1, NORMAL, 1));
                break;
            case BASIC_ALIEN:
            case STRONG_ALIEN:
                if (rand() % 100 < 20) {
                    bullets.push_back(new Bullet(pos.x, pos.y + 2, 1, NORMAL, 1));
                }
                break;
        }
        
        return bullets;
    }
    
    int getPoints() const { return points; }
    AlienType getAlienType() const { return type; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    
    bool collidesWith(int x, int y) {
        if (bigSprite) {
            return bigSprite->collidesWith(x, y);
        }
        return (x == pos.x && y == pos.y);
    }
    
    static char getSymbol(AlienType t) {
        switch(t) {
            case BASIC_ALIEN: return 'B';
            case FAST_ALIEN: return 'F';
            case STRONG_ALIEN: return 'S';
            case SHOOTER_ALIEN: return 'T';
            case BOSS_ALIEN: return 'X';
            case MINION_ALIEN: return 'm';
            default: return 'V';
        }
    }
    
    static int getColor(AlienType t) {
        switch(t) {
            case BASIC_ALIEN: return 4;
            case FAST_ALIEN: return 5;
            case STRONG_ALIEN: return 6;
            case SHOOTER_ALIEN: return 7;
            case BOSS_ALIEN: return 10;
            case MINION_ALIEN: return 8;
            default: return 4;
        }
    }
    
    void draw() override {
        // Gambar sprite besar jika ada
        if (bigSprite) {
            if (health < maxHealth * 0.3) {
                bigSprite->draw(7); // Warna merah saat hampir mati
            } else {
                bigSprite->draw(colorPair);
            }
            
            if (type == BOSS_ALIEN) {
                attron(A_BOLD | A_BLINK);
            }
        }
        
        // Gambar simbol di tengah sprite besar
        if (bigSprite) {
            attron(COLOR_PAIR(colorPair));
            mvaddch(pos.y, pos.x, symbol);
            attroff(COLOR_PAIR(colorPair));
        } else {
            attron(COLOR_PAIR(colorPair));
            mvaddch(pos.y, pos.x, symbol);
            attroff(COLOR_PAIR(colorPair));
        }
        
        if (type == BOSS_ALIEN) {
            attroff(A_BOLD | A_BLINK);
        }
    }
};

// Kelas untuk power-up
class PowerUp : public GameObject {
private:
    int type;
    int fallSpeed;
    
public:
    PowerUp(int x, int y) : GameObject(x, y, '*', 4), fallSpeed(1) {
        type = rand() % 6;
        
        switch(type) {
            case 0: // Life
                symbol = 'H';
                colorPair = 5;
                break;
            case 1: // Laser
                symbol = 'L';
                colorPair = 12;
                break;
            case 2: // Missile
                symbol = 'M';
                colorPair = 13;
                break;
            case 3: // Spread
                symbol = 'S';
                colorPair = 14;
                break;
            case 4: // Pierce
                symbol = 'P';
                colorPair = 15;
                break;
            case 5: // Shield
                symbol = 'D';
                colorPair = 6;
                break;
        }
    }
    
    void update() override {
        pos.y += fallSpeed;
    }
    
    bool isOutOfBounds() const {
        return pos.y >= GAME_HEIGHT;
    }
    
    int getType() const { return type; }
    
    void draw() override {
        attron(A_BOLD);
        attron(COLOR_PAIR(colorPair));
        // Buat power-up lebih besar
        mvaddch(pos.y, pos.x, symbol);
        mvaddch(pos.y + 1, pos.x, 'v');
        attroff(COLOR_PAIR(colorPair));
        attroff(A_BOLD);
    }
};

// Kelas untuk latar bintang
class Star {
private:
    Position pos;
    int speed;
    char symbol;
    
public:
    Star() {
        pos.x = rand() % GAME_WIDTH;
        pos.y = rand() % GAME_HEIGHT;
        speed = rand() % 3 + 1;
        symbol = '.';
        if (speed == 1) symbol = '.';
        else if (speed == 2) symbol = ',';
        else symbol = '\'';
    }
    
    void update() {
        pos.y += speed;
        if (pos.y >= GAME_HEIGHT) {
            pos.y = 0;
            pos.x = rand() % GAME_WIDTH;
        }
    }
    
    void draw() {
        attron(COLOR_PAIR(8));
        mvaddch(pos.y, pos.x, symbol);
        attroff(COLOR_PAIR(8));
    }
};

// Kelas game utama
class SpaceShooterGame {
private:
    Player* player;
    vector<Bullet*> bullets;
    vector<Alien*> aliens;
vector<PowerUp*> powerups;
    Star stars[MAX_STARS];
    
    int level;
    int enemySpawnTimer;
    int powerUpSpawnTimer;
    int gameSpeed;
    bool gameOver;
    bool paused;
    int frameCount;
    int bossSpawned;
    
    // Warna untuk game
    void initColors() {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_YELLOW, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
        init_pair(8, COLOR_WHITE, COLOR_BLACK);
        init_pair(9, COLOR_BLUE, COLOR_BLACK);
        init_pair(10, COLOR_YELLOW, COLOR_BLACK);
        init_pair(11, COLOR_RED, COLOR_BLACK);
        init_pair(12, COLOR_BLUE, COLOR_BLACK);
        init_pair(13, COLOR_RED, COLOR_BLACK);
        init_pair(14, COLOR_YELLOW, COLOR_BLACK);
        init_pair(15, COLOR_CYAN, COLOR_BLACK);
    }
    
    // Spawn alien baru - PERLAMBAT SPAWN RATE
    void spawnAlien() {
        if (aliens.size() < MAX_ENEMIES) {
            int x = rand() % (GAME_WIDTH - 8) + 4;
            AlienType type = BASIC_ALIEN;
            
            // PERLAMBAT SPAWN BOSS DAN MUSUH KUAT
            int chance = rand() % 100;
            
            if (level >= 5 && !bossSpawned && chance < 2) { // Kurangi chance boss
                type = BOSS_ALIEN;
                bossSpawned = true;
            } else if (level > 4 && chance < 8) { // Kurangi chance shooter
                type = SHOOTER_ALIEN;
            } else if (level > 3 && chance < 12) { // Kurangi chance strong
                type = STRONG_ALIEN;
            } else if (level > 2 && chance < 20) {
                type = FAST_ALIEN;
            } else if (level > 1 && chance < 30) {
                type = MINION_ALIEN;
            } else {
                type = BASIC_ALIEN;
            }
            
            aliens.push_back(new Alien(x, 2, type));
        }
    }
    
    // Spawn power-up
    void spawnPowerUp() {
        if (powerups.size() < MAX_POWERUPS && rand() % 100 < 5) { // Kurangi chance power-up
            int x = rand() % (GAME_WIDTH - 4) + 2;
            powerups.push_back(new PowerUp(x, 1));
        }
    }
    
    // Spawn minion untuk boss
    void spawnBossMinions(int bossX) {
        if (aliens.size() < MAX_ENEMIES - 4 && frameCount % 120 == 0) { // Kurangi frekuensi spawn minion
            aliens.push_back(new Alien(bossX - 5, 5, MINION_ALIEN));
            aliens.push_back(new Alien(bossX + 5, 5, MINION_ALIEN));
        }
    }
    
    void cleanBullets() {
        for (auto it = bullets.begin(); it != bullets.end();) {
            if ((*it)->isOutOfBounds()) {
                delete *it;
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void cleanAliens() {
        for (auto it = aliens.begin(); it != aliens.end();) {
            if ((*it)->isOutOfBounds()) {
                if ((*it)->getAlienType() == BOSS_ALIEN) {
                    bossSpawned = false;
                }
                delete *it;
                it = aliens.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void cleanPowerUps() {
        for (auto it = powerups.begin(); it != powerups.end();) {
            if ((*it)->isOutOfBounds()) {
                delete *it;
                it = powerups.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Deteksi tabrakan dengan sprite besar
    void checkCollisions() {
        // Tabrakan peluru pemain dengan alien (DENGAN SPRITE BESAR)
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
            if ((*bulletIt)->getDirection() == -1) {
                bool bulletRemoved = false;
                
                for (auto alienIt = aliens.begin(); alienIt != aliens.end();) {
                    Position bulletPos = (*bulletIt)->getPosition();
                    
                    // Gunakan deteksi tabrakan dengan sprite besar
                    if ((*alienIt)->collidesWith(bulletPos.x, bulletPos.y)) {
                        int damage = (*bulletIt)->getDamage();
                        if ((*alienIt)->takeDamage(damage)) {
                            // BERI SKOR HANYA JIKA BERHASIL MENEMBAK (menghancurkan)
                            player->addScore((*alienIt)->getPoints());
                            
                            if ((*alienIt)->getAlienType() == BOSS_ALIEN) {
                                bossSpawned = false;
                                // Bonus skor untuk boss
                                player->addScore(500);
                                // Spawn banyak power-up sebagai reward
                                for (int i = 0; i < 3; i++) {
                                    spawnPowerUp();
                                }
                            }
                            
                            delete *alienIt;
                            alienIt = aliens.erase(alienIt);
                            
                            // Kurangi chance spawn power-up
                            if (rand() % 100 < 15) {
                                spawnPowerUp();
                            }
                        } else {
                            ++alienIt;
                        }
                        
                        if (!(*bulletIt)->pierce()) {
                            delete *bulletIt;
                            bulletIt = bullets.erase(bulletIt);
                            bulletRemoved = true;
                            break;
                        }
                    } else {
                        ++alienIt;
                    }
                }
                
                if (!bulletRemoved) ++bulletIt;
            } else {
                ++bulletIt;
            }
        }
        
        // Tabrakan peluru alien dengan pemain
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
            if ((*bulletIt)->getDirection() == 1) {
                Position bulletPos = (*bulletIt)->getPosition();
                Position playerPos = player->getPosition();
                
                if (player->collidesWith(bulletPos.x, bulletPos.y)) {
                    player->loseLife();
                    delete *bulletIt;
                    bulletIt = bullets.erase(bulletIt);
                    
                    if (player->getLives() <= 0) {
                        gameOver = true;
                    }
                } else {
                    ++bulletIt;
                }
            } else {
                ++bulletIt;
            }
        }
        
        // Tabrakan alien dengan pemain (DENGAN SPRITE BESAR)
        for (auto alienIt = aliens.begin(); alienIt != aliens.end();) {
            Position alienPos = (*alienIt)->getPosition();
            Position playerPos = player->getPosition();
            
            // Cek semua posisi pemain untuk tabrakan
            bool collision = false;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = 0; dy <= 1; dy++) {
                    if (player->collidesWith(playerPos.x + dx, playerPos.y + dy) &&
                        (*alienIt)->collidesWith(playerPos.x + dx, playerPos.y + dy)) {
                        collision = true;
                        break;
                    }
                }
                if (collision) break;
            }
            
            if (collision) {
                player->loseLife();
                
                if ((*alienIt)->getAlienType() != BOSS_ALIEN) {
                    delete *alienIt;
                    alienIt = aliens.erase(alienIt);
                } else {
                    ++alienIt;
                }
                
                if (player->getLives() <= 0) {
                    gameOver = true;
                }
            } else {
                ++alienIt;
            }
        }
        
        // Tabrakan power-up dengan pemain
        for (auto powerIt = powerups.begin(); powerIt != powerups.end();) {
            Position powerPos = (*powerIt)->getPosition();
            Position playerPos = player->getPosition();
            
            if (abs(powerPos.x - playerPos.x) <= 1 && 
                (powerPos.y == playerPos.y || powerPos.y == playerPos.y + 1)) {
                int type = (*powerIt)->getType();
                int duration = 200; // Kurangi durasi power-up
                
                switch(type) {
                    case 0: player->gainLife(); break;
                    case 1: player->activatePower(duration, LASER); break;
                    case 2: player->activatePower(duration, MISSILE); break;
                    case 3: player->activatePower(duration, SPREAD); break;
                    case 4: player->activatePower(duration, PIERCE); break;
                    case 5: player->activatePower(duration, NORMAL); break;
                }
                
                // Tambah skor kecil untuk mengambil power-up
                player->addScore(10);
                
                delete *powerIt;
                powerIt = powerups.erase(powerIt);
            } else {
                ++powerIt;
            }
        }
    }
    
    // Tembakan pemain
    void playerShoot() {
        if (player->canShoot()) {
            Position pos = player->getPosition();
            BulletType weaponType = (BulletType)player->getWeaponType();
            
            switch(weaponType) {
                case NORMAL:
                    bullets.push_back(new Bullet(pos.x, pos.y - 1, -1, NORMAL, 2));
                    break;
                case LASER:
                    bullets.push_back(new Bullet(pos.x, pos.y - 1, -1, LASER, 3));
                    break;
                case MISSILE:
                    bullets.push_back(new Bullet(pos.x, pos.y - 1, -1, MISSILE, 1));
                    break;
                case SPREAD:
                    bullets.push_back(new Bullet(pos.x, pos.y - 1, -1, SPREAD, 2));
                    bullets.push_back(new Bullet(pos.x - 1, pos.y - 1, -1, SPREAD, 2));
                    bullets.push_back(new Bullet(pos.x + 1, pos.y - 1, -1, SPREAD, 2));
                    break;
                case PIERCE:
                    bullets.push_back(new Bullet(pos.x, pos.y - 1, -1, PIERCE, 2));
                    break;
            }
            
            player->resetCooldown();
        }
    }
    
    // Tembakan alien - PERLAMBAT
    void alienShoot() {
        for (auto alien : aliens) {
            if (alien->canShoot()) {
                vector<Bullet*> newBullets = alien->shoot();
                for (auto bullet : newBullets) {
                    bullets.push_back(bullet);
                }
                
                // Jika boss, spawn minion dengan frekuensi lebih rendah
                if (alien->getAlienType() == BOSS_ALIEN && frameCount % 150 == 0) {
                    spawnBossMinions(alien->getPosition().x);
                }
            }
        }
    }
    
    void drawUI() {
        attron(COLOR_PAIR(9));
        attron(A_BOLD);
        
        // Border game
        for (int i = 0; i < GAME_WIDTH; i++) {
            mvaddch(0, i, '=');
            mvaddch(GAME_HEIGHT, i, '=');
        }
        
        for (int i = 0; i < GAME_HEIGHT; i++) {
            mvaddch(i, 0, '|');
            mvaddch(i, GAME_WIDTH, '|');
        }
        
        mvaddch(0, 0, '+');
        mvaddch(0, GAME_WIDTH, '+');
        mvaddch(GAME_HEIGHT, 0, '+');
        mvaddch(GAME_HEIGHT, GAME_WIDTH, '+');
        
        // Info pemain
        mvprintw(1, 2, "LIVES: %d", player->getLives());
        mvprintw(1, 15, "SCORE: %08d", player->getScore());
        mvprintw(1, 35, "LEVEL: %d", level);
        
        // Info weapon
        if (player->getPowered()) {
            string weaponName;
            switch(player->getWeaponType()) {
                case LASER: weaponName = "LASER"; break;
                case MISSILE: weaponName = "MISSILE"; break;
                case SPREAD: weaponName = "SPREAD"; break;
                case PIERCE: weaponName = "PIERCE"; break;
                default: weaponName = "NORMAL";
            }
            mvprintw(1, 50, "WEAPON: %s", weaponName.c_str());
        }
        
        // Info alien
        mvprintw(2, 2, "ALIENS: %d/%d", aliens.size(), MAX_ENEMIES);
        
        // Cari boss dan tampilkan health bar
        for (auto alien : aliens) {
            if (alien->getAlienType() == BOSS_ALIEN) {
                int health = alien->getHealth();
                int maxHealth = alien->getMaxHealth();
                int barWidth = 20;
                int filled = (health * barWidth) / maxHealth;
                
                mvprintw(2, 50, "BOSS: [");
                for (int i = 0; i < barWidth; i++) {
                    if (i < filled) {
                        attron(COLOR_PAIR(11));
                        mvaddch(2, 57 + i, '#');
                        attroff(COLOR_PAIR(11));
                    } else {
                        mvaddch(2, 57 + i, '.');
                    }
                }
                mvprintw(2, 57 + barWidth, "] %d/%d", health, maxHealth);
                break;
            }
        }
        
        // Kontrol
        mvprintw(GAME_HEIGHT + 1, 2, "CONTROLS: ARROWS/WASD (Move) | SPACE (Shoot) | P (Pause) | Q (Quit)");
        mvprintw(GAME_HEIGHT + 2, 2, "POWER-UPS: H=Health L=Laser M=Missile S=Spread P=Pierce D=Shield");
        
        attroff(A_BOLD);
        attroff(COLOR_PAIR(9));
    }
    
    void drawGameOver() {
        clear();
        
        attron(COLOR_PAIR(10));
        attron(A_BOLD);
        
        // Border
        for (int i = 0; i < GAME_WIDTH; i++) {
            mvaddch(5, i, '=');
            mvaddch(GAME_HEIGHT - 5, i, '=');
        }
        
        for (int i = 5; i < GAME_HEIGHT - 4; i++) {
            mvaddch(i, 10, '|');
            mvaddch(i, GAME_WIDTH - 10, '|');
        }
        
        mvaddch(5, 10, '+');
        mvaddch(5, GAME_WIDTH - 10, '+');
        mvaddch(GAME_HEIGHT - 5, 10, '+');
        mvaddch(GAME_HEIGHT - 5, GAME_WIDTH - 10, '+');
        
        // Pesan game over
        mvprintw(GAME_HEIGHT / 2 - 2, GAME_WIDTH / 2 - 10, "GAME OVER");
        mvprintw(GAME_HEIGHT / 2, GAME_WIDTH / 2 - 15, "FINAL SCORE: %08d", player->getScore());
        mvprintw(GAME_HEIGHT / 2 + 2, GAME_WIDTH / 2 - 12, "Press R to Restart");
        mvprintw(GAME_HEIGHT / 2 + 3, GAME_WIDTH / 2 - 10, "Press Q to Quit");
        
        attroff(A_BOLD);
        attroff(COLOR_PAIR(10));
        
        refresh();
    }
    
    void drawPauseScreen() {
        attron(COLOR_PAIR(10));
        attron(A_BOLD);
        
        mvprintw(GAME_HEIGHT / 2, GAME_WIDTH / 2 - 5, "PAUSED");
        mvprintw(GAME_HEIGHT / 2 + 2, GAME_WIDTH / 2 - 12, "Press P to Resume");
        
        attroff(A_BOLD);
        attroff(COLOR_PAIR(10));
    }
    
public:
    SpaceShooterGame() : level(1), enemySpawnTimer(0), powerUpSpawnTimer(0), 
                         gameSpeed(40), gameOver(false), paused(false), frameCount(0), bossSpawned(false) {
        srand(time(NULL));
        
        initscr();
        cbreak();
        noecho();
        curs_set(0);
        timeout(0);
        keypad(stdscr, TRUE);
        
        initColors();
        
        player = new Player(GAME_WIDTH / 2, GAME_HEIGHT - 5);
        
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i] = Star();
        }
    }
    
    ~SpaceShooterGame() {
        delete player;
        
        for (auto bullet : bullets) delete bullet;
        for (auto alien : aliens) delete alien;
        for (auto powerup : powerups) delete powerup;
        
        endwin();
    }
    
    void processInput() {
        int ch = getch();
        
        switch(ch) {
            case 'q':
            case 'Q':
                gameOver = true;
                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
                player->moveLeft();
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                player->moveRight();
                break;
            case KEY_UP:
            case 'w':
            case 'W':
                player->moveUp();
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                player->moveDown();
                break;
            case ' ':
                if (!paused) {
                    playerShoot();
                }
                break;
            case 'p':
            case 'P':
                paused = !paused;
                break;
            case 'r':
            case 'R':
                if (gameOver) {
                    restartGame();
                }
                break;
        }
    }
    
    void update() {
        if (paused || gameOver) return;
        
        frameCount++;
        
        // Update bintang
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].update();
        }
        
        // Update player power
        player->updatePower();
        
        // Update bullets
        for (auto bullet : bullets) {
            bullet->update();
        }
        
        // Update enemies - PERLAMBAT UPDATE
        if (frameCount % 2 == 0) {
            for (auto alien : aliens) {
                alien->update();
            }
        }
        
        // Update power-ups
        for (auto powerup : powerups) {
            powerup->update();
        }
        
        // PERLAMBAT SPAWN MUSUH
        enemySpawnTimer++;
        if (enemySpawnTimer > 80 - (level * 5)) { // Tambah delay spawn
            spawnAlien();
            enemySpawnTimer = 0;
        }
        
        // PERLAMBAT SPAWN POWER-UP
        powerUpSpawnTimer++;
        if (powerUpSpawnTimer > 400) { // Tambah delay power-up
            spawnPowerUp();
            powerUpSpawnTimer = 0;
        }
        
        // PERLAMBAT TEMBAKAN ALIEN
        if (frameCount % 3 == 0) {
            alienShoot();
        }
        
        // Naik level lebih lambat
        if (player->getScore() > level * 1500) {
            level++;
            if (gameSpeed > 20) gameSpeed -= 2;
        }
        
        cleanBullets();
        cleanAliens();
        cleanPowerUps();
        
        checkCollisions();
    }
    
    void render() {
        clear();
        
        // Gambar bintang
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].draw();
        }
        
        // Gambar semua objek
        for (auto bullet : bullets) bullet->draw();
        for (auto alien : aliens) alien->draw();
        for (auto powerup : powerups) powerup->draw();
        player->draw();
        
        drawUI();
        
        if (paused) {
            drawPauseScreen();
        }
        
        refresh();
    }
    
    void restartGame() {
        level = 1;
        gameOver = false;
        paused = false;
        enemySpawnTimer = 0;
        powerUpSpawnTimer = 0;
        gameSpeed = 40;
        frameCount = 0;
        bossSpawned = false;
        
        for (auto bullet : bullets) delete bullet;
        for (auto alien : aliens) delete alien;
        for (auto powerup : powerups) delete powerup;
        
        bullets.clear();
        aliens.clear();
        powerups.clear();
        
        delete player;
        player = new Player(GAME_WIDTH / 2, GAME_HEIGHT - 5);
    }
    
    void run() {
        while (!gameOver) {
            processInput();
            
            if (!paused) {
                update();
            }
            
            render();
            
            if (gameOver) {
                drawGameOver();
                
                while (true) {
                    int ch = getch();
                    if (ch == 'r' || ch == 'R') {
                        restartGame();
                        break;
                    } else if (ch == 'q' || ch == 'Q') {
                        gameOver = true;
                        break;
                    }
                    Sleep(50);
                }
            }
            
            Sleep(gameSpeed);
        }
    }
};

// Fungsi utama
int main() {
    try {
        SpaceShooterGame game;
        game.run();
    } catch (const exception& e) {
        endwin();
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

