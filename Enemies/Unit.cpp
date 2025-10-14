#include "Unit.h"
#include "../Game.h"
#include <iostream>
#include "../Support/AudioManager.h"
#include "../Player.h" // Thêm include này để có định nghĩa đầy đủ của lớp Player


const float Unit::size = 0.48f;

Unit::Unit(SDL_Renderer* renderer, Vector2D setPos)
    : pos(setPos), speed(1.0f), health(20), maxHealth(20), attackDamage(5), attackRange(0.5f) {

    textureRun = TextureLoader::loadTexture(renderer, "Slime_run.png");
    textureAttack = TextureLoader::loadTexture(renderer, "Slime_attack.png");
    textureHurt = TextureLoader::loadTexture(renderer, "Slime_hurt.png");
    textureDeath = TextureLoader::loadTexture(renderer, "Slime_death.png");
    speed = 0.5f;
    frameTime = 1.0f / 15.0f;
}

void Unit::update(float dT, Level& level, std::vector<std::shared_ptr<Unit>>& listUnits, Player& player) {

    if (state == UnitState::Death) {
        timerDeath.countDown(dT);
        frameTimer += dT;

        if (frameTimer >= frameTime) {
            frameTimer = 0.0f;
            if (frame < getFrameCount() - 1) frame++; // Khong lap lai khi het frame
        }

        if (timerDeath.timeSIsZero()) {
            isdead = true;
        }
        return;
    }

    if (frozen) return;

    timerJustHurt.countDown(dT);
    damageCooldown.countDown(dT);

    frameTimer += dT;
    if (frameTimer >= frameTime) {
        frameTimer = 0.0f;
        if (state != UnitState::Hurt || frame < getFrameCount() - 1) frame++; // Khong lap Hurt
        if (frame >= getFrameCount()) frame = 0;
    }

    if (state == UnitState::Hurt) {
        if (frame >= getFrameCount() - 1) {
            setState(UnitState::Run);
            frame = 0;
        }
        return;
    }

    Vector2D playerPos = player.getPos();
    Vector2D dirVector = playerPos - pos;
    float length = dirVector.magnitude();

    // Dieu chinh trang thai theo khoang cach
    if (length > attackRange) {
        setState(UnitState::Run);
    } else {
        setState(UnitState::Attack);
    }

    // Chong de len nhau
    for (auto& otherUnit : listUnits) {
        if (otherUnit.get() == this) continue; // Bo qua chinh no

        Vector2D otherPos = otherUnit->getPos();
        Vector2D distanceVec = pos - otherPos;
        float distance = distanceVec.magnitude();

        float minDistance = 0.5f; // Khoang cach toi thieu giua cac quai

        // Neu quai qua gan nhau, day chung tach ra
        if (distance < minDistance) {
            Vector2D pushAway = distanceVec.normalize() * (minDistance - distance);
            pos = pos + pushAway * dT; // Day nhe quai ra
        }
    }

    // Di chuyen quai
    if (state == UnitState::Run) {
        if (length > 0) {
            dirVector.normalize();
            pos = pos + dirVector * speed * dT;
        }
    }

    // Cap nhat huong
    if (state == UnitState::Run || state == UnitState::Attack) {
        if (length > 0) {
            dirVector.normalize();
            int newDirection;
            if (abs(dirVector.x) > abs(dirVector.y)) {
                newDirection = (dirVector.x > 0) ? 3 : 2;
            } else {
                newDirection = (dirVector.y > 0) ? 0 : 1;
            }

            if (newDirection != rowIndex) {
                rowIndex = newDirection;
                frame = 0;
            }
        }
    }

    // Tan cong nguoi choi
    if (length < attackRange && damageCooldown.timeSIsZero()) {
        AudioManager::playSound("Data/Sound/monster_attack.mp3");
        Mix_VolumeChunk(AudioManager::getSound("Data/Sound/monster_attack.mp3"), 50);

        player.removeHealth(attackDamage);
        damageCooldown.resetToMax();
    }
}

void Unit::draw(SDL_Renderer* renderer, int tileSize, Vector2D cameraPos) {

    if (!renderer || isDead()) return;

    SDL_Texture* currentTexture = getTextureForState();
    if (!currentTexture) return;

    int columns = 8; // Mac dinh 8 cot

    // Tu dong tinh toan so cot dua tren trang thai Hurt
    if (state == UnitState::Hurt) {
        int hurtFrameCount = getFrameCount(); // Lay so frame cua Hurt

        // Neu la trang thai Hurt thi tu dong xac dinh so cot
        if (hurtFrameCount % 6 == 0) {
            columns = 6; // Neu chia het cho 6 -> Orc
        } else if (hurtFrameCount % 4 == 0) {
            columns = 4; // Neu chia het cho 4 -> Vampire
        }
    }

    int totalFrames = getFrameCount();
    frame = std::max(0, std::min(frame, totalFrames - 1));

    int row = frame / columns;
    int column = frame % columns;

    // Kiem tra frame hop le
    if (row * columns + column >= totalFrames) {
        frame = 0;
        row = 0;
        column = 0;
    }

    SDL_Rect srcRect = { column * frameWidth, rowIndex * frameHeight, frameWidth, frameHeight };
    SDL_Rect destRect = {
        (int)(pos.x * tileSize) - frameWidth / 2 - (int)(cameraPos.x * tileSize),
        (int)(pos.y * tileSize) - frameHeight / 2 - (int)(cameraPos.y * tileSize),
        frameWidth, frameHeight
    };

    SDL_RenderCopy(renderer, currentTexture, &srcRect, &destRect);
}

bool Unit::checkOverlap(Vector2D posOther, float sizeOther) {
    return (posOther - pos).magnitude() <= (sizeOther + size) / 2.0f;
}

bool Unit::isAlive() { return !isdead; }
bool Unit::isDead() { return isdead; }
Vector2D Unit::getPos() { return pos; }

void Unit::takeDamage(int damage, Game* game) {
    if (state == UnitState::Death) return;

    health -= damage;
    if (health <= 0) {
        if (state != UnitState::Death) {
            AudioManager::playSound("Data/Sound/monster_die.mp3");
            Mix_VolumeChunk(AudioManager::getSound("Data/Sound/monster_die.mp3"), 50);
            setState(UnitState::Death);
            timerDeath.resetToMax();
            frame = 0;
            frameTimer = 0.0f;
            if (game) {
                SDL_Renderer* renderer = game->getRenderer();
                int numCoins = rand() % 3 + 1;
                for (int i = 0; i < numCoins; i++) {
                    AudioManager::playSound("Data/Sound/coin_drop.mp3");
                    Mix_VolumeChunk(AudioManager::getSound("Data/Sound/coin_drop.mp3"), 50);
                    Vector2D coinOffset((rand() % 10 - 5) * 0.1f, (rand() % 10 - 5) * 0.1f);
                    game->coins.push_back(std::make_shared<Coin>(pos + coinOffset, renderer));
                }
            }
        }
    } else if (state != UnitState::Hurt) {
        setState(UnitState::Hurt);
        timerJustHurt.resetToMax();
        frame = 0;
        frameTimer = 0.0f;
    }
}

void Unit::setState(UnitState newState) {
    if (state == newState) return;

    state = newState;
    frame = 0;

    switch (state) {
        case UnitState::Run: frameTime = 0.15f; break;
        case UnitState::Attack: frameTime = 0.1f; break;
        case UnitState::Hurt: frameTime = 0.2f; break;
        case UnitState::Death: frameTime = 0.2f; break;
        default: frameTime = 0.15f; break;
    }
}

SDL_Texture* Unit::getTextureForState() {
    switch (state) {
        case UnitState::Run: return textureRun;
        case UnitState::Attack: return textureAttack;
        case UnitState::Hurt: return textureHurt;
        case UnitState::Death: return textureDeath;
        default: return textureRun;
    }
}

int Unit::getFrameCount() {
    switch (state) {
        case UnitState::Run: return 8;
        case UnitState::Attack: return 10;
        case UnitState::Hurt: return 20;
        case UnitState::Death: return 40;
        default: return 8;
    }
}
