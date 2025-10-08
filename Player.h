#pragma once
#include "SDL.h"
#include "Support/Vector2D.h"
#include "Support/TextureLoader.h"
#include "Game.h"
#include <vector>
#include "Level.h"
#include "Support/AudioManager.h"

class Unit;
class Game;  // Khai bao truoc de tranh loi
class Level;



enum class PlayerState {
    IdleLeft,
    IdleRight,
    RunLeft,
    Attack1,
    RunRight,
    Attack1Left,
    Hurt,  // Them trang thai bi danh
    Death,

};

class Player
{
public:
    ~Player();  // Destructor de giai phong texture

    Player(Game* gamePtr, SDL_Renderer* renderer, Vector2D startPos);
    void handleInput(const Uint8* keyState, SDL_Renderer* renderer);
    void update(float dT, std::vector<std::shared_ptr<Unit>>& listUnits, SDL_Renderer* renderer, Level& level);
    void draw(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos);
    Vector2D getPos(){return pos;}

    int getCurrentHP() { return currentHP; }
    int getMaxHP() { return maxHP; }
    float getCurrentMP() { return currentMP; }
    float getMaxMP() { return maxMP; }

    void removeHealth(int damage);

    void increaseHealth();

    int attackDamage = 10;
    int maxHP = 100;
    int currentHP = 100;
    float maxMP = 50.0;
    float currentMP = 50;
    int coin = 0;

    void levelUp();

    int getCoins() const { return coin; }

    int getLastDirection(){return lastDirection;}

    bool isDead = false;


    Timer deathTimer = Timer(2.0f);

    int frame = 0;
    int frameCount = 8; // 8 frame moi animation
    float frameTime = 1.0f / 15.0f;
    float frameTimer = 0.0f;
    int level = 1;

    void reset();


private:
    Vector2D pos;
    Vector2D direction;
    float speed = 3.0f;
    SDL_Texture* textureIdleLeft = nullptr;
    SDL_Texture* textureIdleRight = nullptr;
    SDL_Texture* textureRunRight = nullptr;
    SDL_Texture* textureAttack1 = nullptr;
    SDL_Texture* textureRunLeft = nullptr;
    SDL_Texture* textureAttack1Left = nullptr;
    SDL_Texture* textureHurt = nullptr;
    SDL_Texture* textureDeath = nullptr;




    Timer shootCooldown = Timer(0.7f); // Khoang cach giua cac lan ban






private:
    PlayerState state = PlayerState::IdleRight; // Mac dinh la dung yen
    int spriteWidth = 2000;  // Kich thuoc toan bo spritesheet
    int spriteHeight = 250;
    int frameWidth = spriteWidth / 8;  // Kich thuoc cua mot frame = 2000/8 = 250px
    int frameHeight = spriteHeight;
    int lastDirection;

    Vector2D smoothPos; // Vi tri noi suy

    bool isAttacking = false;  // Kiem soat khi nao player dang tan cong


    float attackRange = 1.5f;  // Pham vi danh can chien


    Game* game;


    Timer damageCooldown = Timer(0.7f); // 1 giay giua moi lan nhan damage


    Timer hurtTimer { 0.5f, 0.0f }; //  Nhap nhay 0.5 giay khi bi danh


    PlayerState prevStateBeforeHurt;
    int prevFrameBeforeHurt;



};
