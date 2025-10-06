#include "FireEffect.h"
#include <iostream>

FireEffect::FireEffect(SDL_Renderer* renderer, Vector2D setPos) : pos(setPos) {
    textureFire = TextureLoader::loadTexture(renderer, "Fire_III_Flame_C_80x48.png"); // Sprite sheet 6 frame
    if (!textureFire) {
        std::cout << "Failed to load Fire_effect.png!" << std::endl;
    }
    durationTimer.resetToMax();
    damageCooldown.resetToMax();
}

void FireEffect::update(float dT, std::vector<std::shared_ptr<Unit>>& listUnits) {
    durationTimer.countDown(dT);
    damageCooldown.countDown(dT);

    // Cap nhat animation
    frameTimer += dT;
    if (frameTimer >= frameTime) {
        frameTimer = 0.0f;
        frame = (frame + 1) % frameCount;
    }

    // Gây sát thuong cho quái trong pham vi
    if (damageCooldown.timeSIsZero()) {
        for (auto& unit : listUnits) {
            if (unit && unit->isAlive()) {
                float distance = (unit->getPos() - pos).magnitude();
                if (distance <= radius) {
                    unit->takeDamage(damage, nullptr); // Gây sát thuong
                }
            }
        }
        damageCooldown.resetToMax(); // Reset cooldown sát thuong
    }
}

void FireEffect::draw(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos) {
    if (!textureFire || durationTimer.timeSIsZero()) return;

    SDL_Rect srcRect = { frame * frameWidth, 0, frameWidth, frameHeight };
    SDL_Rect destRect = {
        (int)(pos.x * tileSize) - frameWidth / 2 - (int)(cameraPos.x * tileSize),
        (int)(pos.y * tileSize) - frameHeight / 2 - (int)(cameraPos.y * tileSize),
        frameWidth * 2, frameHeight * 2 // Phóng to gap doi (có the diều chinh)
    };

    SDL_RenderCopy(renderer, textureFire, &srcRect, &destRect);
}
