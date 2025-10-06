#include "Boss.h"
#include <iostream>
#include "../Support/AudioManager.h"

Boss::Boss(SDL_Renderer* renderer, Vector2D spawnPos)
    : Unit(renderer, spawnPos), renderer_(renderer) {
    health = 500;
    maxHealth = 500;
    attackDamage = 40;
    speed = 0.5f;

    textureRun = TextureLoader::loadTexture(renderer, "Boss_run.png");
    textureAttack = TextureLoader::loadTexture(renderer, "Boss_attack.png");
    textureHurt = TextureLoader::loadTexture(renderer, "Boss_hurt.png");
    textureDeath = TextureLoader::loadTexture(renderer, "Boss_death.png");
    idleTexture = TextureLoader::loadTexture(renderer, "Boss_idle.png");
    spawnTexture = TextureLoader::loadTexture(renderer, "Boss_spawn.png");

    loadAnimations(spawnTexture, spawnFrames, 11);
    loadAnimations(idleTexture, idleFrames, 6);
    loadAnimations(textureRun, runFrames, 6);
    loadAnimations(textureAttack, attackFrames, 10);
    loadAnimations(textureHurt, hurtFrames, 4);
    loadAnimations(textureDeath, deathFrames, 5);

    state = UnitState::Idle;
    isSpawning = true;
    spawnTimer = 2.0f;
    frameTimeBoss = 2.0f / 11.0f;
    currentFrame = 0;
}

void Boss::loadAnimations(SDL_Texture* spriteSheet, std::vector<SDL_Rect>& frames, int frameCount) {
    int spriteWidth, spriteHeight;
    SDL_QueryTexture(spriteSheet, NULL, NULL, &spriteWidth, &spriteHeight);
    int frameHeight = 120;

    frames.clear();
    for (int i = 0; i < frameCount; i++) {
        SDL_Rect frame = {0, i * frameHeight, spriteWidth, frameHeight};
        frames.push_back(frame);
    }
}

void Boss::update(float dT, Level& level, std::vector<std::shared_ptr<Unit>>& listUnits, Player& player) {
    animationTimer += dT;
    summonTimer += dT;

    // Xu ly trang thai Spawning
    if (isSpawning) {
        spawnTimer -= dT;
        if (animationTimer >= frameTimeBoss && currentFrame < spawnFrames.size() - 1) {
            animationTimer = 0.0f;
            currentFrame++;
        }
        if (spawnTimer <= 0.0f) {
            isSpawning = false;
            state = UnitState::Run;
            currentFrame = 0;
            summonTimer = 0.0f;
        }
        return;
    }

    // Xu ly trang thai Hurt
    if (state == UnitState::Hurt) {
        hurtTimer -= dT;
        if (animationTimer >= frameTimeBoss && currentFrame < hurtFrames.size() - 1) {
            animationTimer = 0.0f;
            currentFrame++;
        }
        if (hurtTimer <= 0.0f) {
            state = previousState; // Quay lai trang thai truoc do
            currentFrame = 0;
            animationTimer = 0.0f;
        }
        return;
    }

    // Xu ly cac trang thai khac (Idle, Death, Attack, Run)
    if (summonTimer >= 5.0f && state != UnitState::Death) {
        AudioManager::playSound("Data/Sound/boss_roar.mp3");
        Mix_VolumeChunk(AudioManager::getSound("Data/Sound/boss_roar.mp3"), 50);
        isIdle = true;
        isSummoning = true;
        summonTimer = 0.0f;
        summonMinions(renderer_, level, listUnits);
        idleTimer = 0.0f;
        currentFrame = 0;
        state = UnitState::Idle;
        return;
    }

    if (isIdle) {
        idleTimer += dT;
        if (animationTimer >= frameTimeBoss) {
            animationTimer = 0.0f;
            currentFrame = (currentFrame + 1) % idleFrames.size();
        }
        if (idleTimer >= idleDuration) {
            isIdle = false;
            isSummoning = false;
            state = UnitState::Run;
            currentFrame = 0;
        }
        return;
    }

    if (state == UnitState::Death) {
        if (!hurtAnimationFinished) { // Chi cap nhat neu animation chua xong
            if (animationTimer >= frameTimeBoss && currentFrame < deathFrames.size() - 1) {
                animationTimer = 0.0f;
                currentFrame++;
            } else if (currentFrame >= deathFrames.size() - 1) {
                hurtAnimationFinished = true; // Danh dau animation da hoan tat
                isdead = true; // Boss chinh thuc chet
            }
        }
        return;
    }

    Vector2D playerPos = player.getPos();
    Vector2D dirVector = playerPos - pos;
    float distance = dirVector.magnitude();

    if (isAttacking) {
        attackTimer += dT;
        if (attackTimer >= frameTimeBoss && attackFrame < attackFrames.size() - 1) {
            attackTimer = 0.0f;
            attackFrame++;
        } else if (attackFrame >= attackFrames.size() - 1) {
            isAttacking = false;
            attackFrame = 0;
            if (distance <= 1.5f && !player.isDead) {
                player.removeHealth(attackDamage);
            }
            state = UnitState::Run;
        }
    } else if (health <= 0) {
        state = UnitState::Death;
        currentFrame = 0;
    } else if (distance > 1.5f) {
        dirVector.normalize();
        pos += dirVector * speed * dT;
        state = UnitState::Run;
        if (animationTimer >= frameTimeBoss) {
            animationTimer = 0.0f;
            currentFrame = (currentFrame + 1) % runFrames.size();
        }
    } else {
        isAttacking = true;
        attackTimer = 0.0f;
        attackFrame = 0;
        state = UnitState::Attack;
    }
}

void Boss::draw(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos) {
    if (!renderer || isDead()) return;

    SDL_Texture* currentTexture = nullptr;
    SDL_Rect currentFrameRect;

    if (isSpawning) {
        currentTexture = spawnTexture;
        currentFrameRect = spawnFrames[currentFrame];
    } else {
        switch (state) {
            case UnitState::Run: currentTexture = textureRun; currentFrameRect = runFrames[currentFrame]; break;
            case UnitState::Attack: currentTexture = textureAttack; currentFrameRect = attackFrames[attackFrame]; break;
            case UnitState::Hurt:
                currentTexture = textureHurt;
                currentFrameRect = hurtFrames[currentFrame];
                break;
            case UnitState::Death: currentTexture = textureDeath; currentFrameRect = deathFrames[currentFrame]; break;
            case UnitState::Idle: default: currentTexture = idleTexture; currentFrameRect = idleFrames[currentFrame]; break;
        }
    }

    if (currentTexture) {
        int xPos = std::round((pos.x - cameraPos.x) * tileSize) - (frameWidth / 2);
        int yPos = std::round((pos.y - cameraPos.y) * tileSize) - (frameHeight / 2);
        SDL_Rect destRect = {xPos, yPos, frameWidth * 2, frameHeight * 2};
        SDL_RenderCopy(renderer, currentTexture, &currentFrameRect, &destRect);
    }

    if (isAlive()) {
        drawHealthBar(renderer, tileSize, cameraPos);
    }
}

void Boss::summonMinions(SDL_Renderer* renderer, Level& level, std::vector<std::shared_ptr<Unit>>& listUnits) {
    if (!isSummoning || !renderer) return;

    int numMinions = 3;
    float summonRadius = 2.0f; // 2 don vi luoi logic

    for (int i = 0; i < numMinions; ++i) {
        float offsetX = (rand() % 200) / 100.0f - 1.0f; // -1.0 den 1.0
        float offsetY = (rand() % 200) / 100.0f - 1.0f; // -1.0 den 1.0
        Vector2D summonPos = pos + Vector2D(offsetX * summonRadius, offsetY * summonRadius);
        summonPos.x = std::max(0.5f, std::min(summonPos.x, (float)level.GetX() - 0.5f));
        summonPos.y = std::max(0.5f, std::min(summonPos.y, (float)level.GetY() - 0.5f));
        listUnits.push_back(std::make_shared<Bat>(renderer, summonPos));
    }
    isSummoning = false;
}

void Boss::drawHealthBar(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos) {
    const int barWidth = 200;  // Chieu rong thanh mau (pixel)
    const int barHeight = 5;  // Chieu cao thanh máu

    // Tinh vi tri thanh mau dua tren vi tri boss
    int barX = ((int)(pos.x * tileSize) - barWidth / 2 - (int)(cameraPos.x * tileSize)) + 60;
    int barY = (int)(pos.y * tileSize) - frameHeight / 2 - (int)(cameraPos.y * tileSize);

    SDL_Rect bg = { barX, barY, barWidth, barHeight };
    SDL_Rect bar = { barX, barY, (int)(barWidth * health / maxHealth), barHeight };

    // Ve vien
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bg);

    // Ve nen
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &bg);

    // Ve thanh mau
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &bar);
}

void Boss::takeDamage(int damage, Game* game) {
    if (state == UnitState::Death || isSpawning) return; // Khong nhan sat thuong khi chet hoac dang spawn

    health -= damage;

    if (health > 0 && state != UnitState::Hurt) {
        previousState = state; // Luu trang thai truoc do
        state = UnitState::Hurt;
        hurtTimer = hurtDuration; // Dat thoi gian bi thuong
        currentFrame = 0; // Bat dau animation Hurt tu frame 0
        animationTimer = 0.0f;
        AudioManager::playSound("Data/Sound/boss_hurt.mp3"); // Them am thanh neu co
    } else if (health <= 0) {
        state = UnitState::Death;
        currentFrame = 0;
        animationTimer = 0.0f;
    }
}







