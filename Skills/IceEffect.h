#pragma once
#include "SDL.h"
#include "../Support/Vector2D.h"
#include "../Support/TextureLoader.h"
#include "../Support/Timer.h"
#include <vector>
#include "../Enemies/Unit.h"

enum class IceState {
    Appear,    // Trang thai xuat hien
    Active,    // Trang thai hoat dong (dong bang quai)
    Disappear  // Trang thai bien mat
};

class IceEffect {
public:
    IceEffect(SDL_Renderer* renderer, Vector2D setPos);
    void update(float dT, std::vector<std::shared_ptr<Unit>>& listUnits);
    void draw(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos);
    bool isFinished()  { return state == IceState::Disappear && durationTimer.timeSIsZero(); }

private:
    Vector2D pos;
    IceState state = IceState::Appear;

    SDL_Texture* textureAppear = nullptr;    // Sprite sheet khi xuat hien
    SDL_Texture* textureActive = nullptr;    // Sprite sheet khi hoat dong
    SDL_Texture* textureDisappear = nullptr; // Sprite sheet khi bien mat

    Timer durationTimer = Timer(0.5f); // Thoi gian moi trang thai (0.5s x 3 = 1.5s tong)
    Timer freezeTimer = Timer(2.0f);   // Thoi gian dong băng quai (2 giây)
    float frameTime = 0.1f;            // Moi frame 0.1 giay
    float frameTimer = 0.0f;
    int frame = 0;
    int frameCount = 8;                // Gia su moi sprite sheet có 6 frame
    int frameWidth = 256/8;          // 480px chia 6 = 80px moi frame
    int frameHeight = 32;              // Chieu cao sprite sheet (48px)

    float radius = 20.0f;               // Pham vi dong bang
    std::vector<std::shared_ptr<Unit>> frozenUnits; // Danh sách quái bi dóng bang
};
