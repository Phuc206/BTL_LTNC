#pragma once
#include "SDL.h"
#include "../Support/Vector2D.h"
#include "../Support/TextureLoader.h"
#include "../Support/Timer.h"
#include <vector>
#include "../Enemies/Unit.h"
class Unit;

class FireEffect {
public:
    FireEffect(SDL_Renderer* renderer, Vector2D setPos);
    void update(float dT, std::vector<std::shared_ptr<Unit>>& listUnits);
    void draw(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos);
    bool isFinished(){ return durationTimer.timeSIsZero(); }

private:
    Vector2D pos;
    SDL_Texture* textureFire = nullptr;
    Timer durationTimer = Timer(4.0f) ; // Ton tai 1 giây
    float frameTime = 0.1f; // Moi frame 0.1 giây
    float frameTimer = 0.0f;
    int frame = 0;
    int frameCount = 6; // 6 frame trong sprite sheet
    int frameWidth = 480/6; // Gia su sprite sheet rong 192px, chia 6 = 32px moi frame
    int frameHeight = 48; // Chieu cao sprite (co the dieu chinh)

    float damage = 10.0f; // Sát thuong gây ra
    float radius = 2.0f; // Pham vi vùng lua
    Timer damageCooldown = Timer(1.0f); // Gây sát thuong moi 0.5 giây
};
