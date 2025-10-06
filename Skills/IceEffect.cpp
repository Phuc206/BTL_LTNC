#include "IceEffect.h"
#include <iostream>

IceEffect::IceEffect(SDL_Renderer* renderer, Vector2D setPos) : pos(setPos) {
    textureAppear = TextureLoader::loadTexture(renderer, "Ice VFX 2 Start.png");    // 480x48, 6 frame
    textureActive = TextureLoader::loadTexture(renderer, "Ice VFX 2 Active.png");    // 480x48, 6 frame
    textureDisappear = TextureLoader::loadTexture(renderer, "Ice VFX 2 Ending.png"); // 480x48, 6 frame

    if (!textureAppear || !textureActive || !textureDisappear) {
        std::cout << "Failed to load Ice effect textures!" << std::endl;
    }
    durationTimer.resetToMax();
    freezeTimer.resetToMax();
}

void IceEffect::update(float dT, std::vector<std::shared_ptr<Unit>>& listUnits) {
    durationTimer.countDown(dT);
    freezeTimer.countDown(dT);

    // Cap nhat animation
    frameTimer += dT;
    if (frameTimer >= frameTime) {
        frameTimer = 0.0f;
        frame = (frame + 1) % frameCount;
    }

    // Chuyen trang thai
    if (durationTimer.timeSIsZero()) {
        if (state == IceState::Appear) {
            state = IceState::Active;
            durationTimer = Timer(4.0f); // Reset cho trang thái Active
            durationTimer.resetToMax();
        } else if (state == IceState::Active) {
            state = IceState::Disappear;
            durationTimer = Timer(0.5f); // Reset cho trang thai Disappear
            durationTimer.resetToMax();
        }
    }

    // Dong bang quai trong trang thai Active
    if (state == IceState::Active && !freezeTimer.timeSIsZero()) {
        for (auto& unit : listUnits) {
            if (unit && unit->isAlive() && !unit->isFrozen()) { // Chi dong bang neu chua bi dong
                float distance = (unit->getPos() - pos).magnitude();
                if (distance <= radius) {
                    unit->setFrozen(true); // Dong bang quai
                    unit->takeDamage(5, nullptr);
                    frozenUnits.push_back(unit); // Them vao danh sach
                }
            }
        }
    }


    // Giai phong quai khi het thoi gian dong bang
    if (freezeTimer.timeSIsZero() && !frozenUnits.empty()) {
        for (auto& unit : frozenUnits) {
            if (unit) unit->setFrozen(false); // Giai phong quai
        }
        frozenUnits.clear();
    }


}

void IceEffect::draw(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos) {
    if (isFinished()) return;

    SDL_Texture* currentTexture = nullptr;
    switch (state) {
        case IceState::Appear: currentTexture = textureAppear; break;
        case IceState::Active: currentTexture = textureActive; break;
        case IceState::Disappear: currentTexture = textureDisappear; break;
    }
    if (!currentTexture) return;

    SDL_Rect srcRect = { frame * frameWidth, 0, frameWidth, frameHeight };
    SDL_Rect destRect = {
        (int)(pos.x * tileSize) - frameWidth / 2 - (int)(cameraPos.x * tileSize),
        (int)(pos.y * tileSize) - frameHeight / 2 - (int)(cameraPos.y * tileSize),
        frameWidth * 4, frameHeight * 4 // Phong to gap doi (160x96px)
    };

    SDL_RenderCopy(renderer, currentTexture, &srcRect, &destRect);
}
