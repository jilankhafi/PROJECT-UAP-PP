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
