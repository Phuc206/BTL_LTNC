#include "Level.h"
#include <iostream>
#include "Player.h"
#include "Support/AudioManager.h"
#include <queue>
#include <algorithm>

Level::Level(SDL_Renderer* renderer, int setTileCountX, int setTileCountY)
    : tileCountX(setTileCountX), tileCountY(setTileCountY),
      targetX(setTileCountX / 2), targetY(setTileCountY / 2) {
    lastPotionSpawnTime = std::chrono::steady_clock::now() - std::chrono::seconds(15);
    textureGrass = TextureLoader::loadTexture(renderer, "grass.jpg");
    potionHealthTexture = TextureLoader::loadTexture(renderer, "potion.png");
    potionManaTexture = TextureLoader::loadTexture(renderer, "Health_potion.png");

    // Load do vat trang tri
    decorTextures.push_back(TextureLoader::loadTexture(renderer, "Autumn_tree1.png"));
    decorTextures.push_back(TextureLoader::loadTexture(renderer, "Autumn_tree1.png"));
    decorTextures.push_back(TextureLoader::loadTexture(renderer, "Fruit_tree1.png"));

    // Them dong nay de tao vi tri cho vat trang tri
    generateDecorPositions();

    // Them dich vao cac goc
    int xMax = tileCountX - 1;
    int yMax = tileCountY - 1;
    setTileType(0, 0, TileType::enemySpawner);
    setTileType(xMax, 0, TileType::enemySpawner);
    setTileType(0, yMax, TileType::enemySpawner);
    setTileType(xMax, yMax, TileType::enemySpawner);

    calculateFlowField();
    spawnPotions();
}

void Level::generateDecorPositions() {
    decorPositions.clear();
    // So luong do trang tri
    int numDecor = 7;
    // Khoang cach toi thieu giua cac do trang tri (don vi tile)
    float minSpacing = 3.0f;

    for (int i = 0; i < numDecor; i++) {
        bool validPosition = false;
        Vector2D newDecor;

        // Lap lai cho den khi tim duoc vi tri hop le
        while (!validPosition) {
            int randX = rand() % tileCountX;
            int randY = rand() % tileCountY;
            newDecor = Vector2D(randX, randY);
            validPosition = true;

            // Kiem tra khoang cach voi cac vat trang tri da dat
            for (auto& decor : decorPositions) {
                if ((decor - newDecor).magnitude() < minSpacing) {
                    validPosition = false;
                    break;
                }
            }
        }
        decorPositions.push_back(newDecor);
    }
}

void Level::draw(SDL_Renderer* renderer, int tileSize, float camX, float camY) {
    if (!renderer) {
        std::cout << "Error: Renderer is null in Level::draw" << std::endl;
        return;
    }

    if (std::chrono::steady_clock::now() - lastPotionSpawnTime >= std::chrono::seconds(15)) {
        spawnPotions();
        lastPotionSpawnTime = std::chrono::steady_clock::now();
    }

    if (!textureGrass) {
        std::cout << "Error: textureGrass is null!" << std::endl;
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderFillRect(renderer, nullptr);
    } else {
        for (int y = 0; y < tileCountY; y++) {
            for (int x = 0; x < tileCountX; x++) {
                // Su dung std::round de can chinh chinh xac
                int xPos = std::round(x * tileSize - camX);
                int yPos = std::round(y * tileSize - camY);
                // Tang kich thuoc tile them 1 pixel de che khe ho
                SDL_Rect rect = {xPos, yPos, tileSize + 1, tileSize + 1};
                SDL_RenderCopy(renderer, textureGrass, NULL, &rect);
            }
        }
    }
    // Ve potion
    drawPotions(renderer, camX, camY);
}

void Level::spawnPotions() {
    int numPotions = 2;
    float minSpacing = 3.0f;
    int maxAttempts = 100;

    for (int i = 0; i < numPotions; i++) {
        bool validPosition = false;
        Vector2D newPotionPos;

        int attempts = 0;
        while (!validPosition && attempts < maxAttempts) {
            int randX = rand() % tileCountX;
            int randY = rand() % tileCountY;
            newPotionPos = Vector2D(randX, randY);
            validPosition = true;
            attempts++;

            // Kiem tra vi tri khong phai enemySpawner
            if (getTileType(randX, randY) == TileType::enemySpawner) {
                validPosition = false;
                continue;
            }

            // Kiem tra khoang cach voi cac potion khac
            for (auto& potion : potionPositions) {
                if ((potion - newPotionPos).magnitude() < minSpacing) {
                    validPosition = false;
                    break;
                }
            }
            // Neu tim thay vi tri hop le
            if (validPosition) {
                potionPositions.push_back(newPotionPos);
                // Random loai potion
                SDL_Texture* potionTexture = (rand() % 2 == 0) ? potionHealthTexture : potionManaTexture;
                potionTextures.push_back(potionTexture);
                std::cout << "Potion spawned at: (" << newPotionPos.x << ", " << newPotionPos.y << ")\n";
            }
        }
    }
}

Vector2D Level::getRandomEnemySpawnerLocation() {
    // Tao danh sach o quai duoc spawn
    std::vector<int> listSpawnerIndices;
    for (int count = 0; static_cast<size_t>(count) < listTiles.size(); count++) {
        auto& tileSelected = listTiles[count];
        if (tileSelected.type == TileType::enemySpawner)
            listSpawnerIndices.push_back(count);
    }
    // Neu co 1 hoac nhieu con duoc spawn thi chon ngau nhien 1 con o vi tri trung tam o gach
    if (!listSpawnerIndices.empty()) {
        int index = listSpawnerIndices[rand() % listSpawnerIndices.size()];
        return Vector2D((float)(index % tileCountX) + 0.5f, (float)(index / tileCountX) + 0.5f);
    }
    return Vector2D(0.5f, 0.5f);
}

Level::TileType Level::getTileType(int x, int y) {
    int index = x + y * tileCountX;
    if (index > -1 && static_cast<size_t>(index) < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY)
        return listTiles[index].type;
    return TileType::empty;
}

void Level::setTileType(int x, int y, TileType tileType) {
    int index = x + y * tileCountX;
    if (index > -1 && static_cast<size_t>(index) < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY) {
        listTiles[index].type = tileType;
        calculateFlowField();
    }
}

void Level::calculateFlowField() {
    // Dam bao cac con quai o bounds
    int indexTarget = targetX + targetY * tileCountX;
    if (indexTarget > -1 && static_cast<size_t>(indexTarget) < listTiles.size() &&
        targetX > -1 && targetX < tileCountX &&
        targetY > -1 && targetY < tileCountY) {
        // Reset lai cai flow field
        for (auto& tileSelected : listTiles) {
            tileSelected.flowDirectionX = 0;
            tileSelected.flowDirectionY = 0;
            tileSelected.flowDistance = flowDistanceMax;
        }
        // Tinh toan cai flow field
        calculateDistances();
        calculateFlowDirections();
    }
}

void Level::calculateDistances() {
    int indexTarget = targetX + targetY * tileCountX;
    // Tao mot hang doi chua cac chi so can duoc kiem tra.
    std::queue<int> listIndicesToCheck;
    // Dat gia tri dong chay (flow) cua o muc tieu thanh 0 va them no vao hang doi.
    listTiles[indexTarget].flowDistance = 0;
    listIndicesToCheck.push(indexTarget);
    // Do lech cua cac o lan can can duoc kiem tra.
    const int listNeighbors[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };
    // Lap qua hang doi va gan khoang cach cho tung o.
    while (!listIndicesToCheck.empty()) {
        int indexCurrent = listIndicesToCheck.front();
        listIndicesToCheck.pop();
        // Kiem tra tung o lan can
        for (int count = 0; count < 4; count++) {
            int neighborX = listNeighbors[count][0] + indexCurrent % tileCountX;
            int neighborY = listNeighbors[count][1] + indexCurrent / tileCountY;
            int indexNeighbor = neighborX + neighborY * tileCountX;
            // Dam bao rang o lan can ton tai va khong phai la tuong.
            if (indexNeighbor > -1 && static_cast<size_t>(indexNeighbor) < listTiles.size() &&
                neighborX > -1 && neighborX < tileCountX &&
                neighborY > -1 && neighborY < tileCountY &&
                listTiles[indexNeighbor].type != TileType::wall) {
                // Kiem tra xem o do da duoc gan khoang cach hay chua.
                if (listTiles[indexNeighbor].flowDistance == flowDistanceMax) {
                    // Neu chua thi gan khoang cach cho no va them vao hang doi.
                    listTiles[indexNeighbor].flowDistance = listTiles[indexCurrent].flowDistance + 1;
                    listIndicesToCheck.push(indexNeighbor);
                }
            }
        }
    }
}

void Level::calculateFlowDirections() {
    // Do lech cua cac o lan can can duoc kiem tra.
    const int listNeighbors[][2] = {
        {-1, 0}, {-1, 1}, {0, 1}, {1, 1},
        {1, 0}, {1, -1}, {0, -1}, {-1, -1}
    };
    for (int indexCurrent = 0; static_cast<size_t>(indexCurrent) < listTiles.size(); indexCurrent++) {
        // Dam bao rang o da duoc gan gia tri khoang cach.
        if (listTiles[indexCurrent].flowDistance != flowDistanceMax) {
            // Dat khoang cach tot nhat bang khoang cach cua o hien tai.
            unsigned char flowFieldBest = listTiles[indexCurrent].flowDistance;
            for (int count = 0; count < 8; count++) {
                int offsetX = listNeighbors[count][0];
                int offsetY = listNeighbors[count][1];
                int neighborX = offsetX + indexCurrent % tileCountX;
                int neighborY = offsetY + indexCurrent / tileCountY;
                int indexNeighbor = neighborX + neighborY * tileCountX;
                // Dam bao lan can ton tai
                if (indexNeighbor > -1 && static_cast<size_t>(indexNeighbor) < listTiles.size() &&
                    neighborX > -1 && neighborX < tileCountX &&
                    neighborY > -1 && neighborY < tileCountY) {
                    // Neu khoang cach cua o lan can hien tai thap hon khoang cach tot nhat, thi su dung no.
                    if (listTiles[indexNeighbor].flowDistance < flowFieldBest) {
                        flowFieldBest = listTiles[indexNeighbor].flowDistance;
                        listTiles[indexCurrent].flowDirectionX = offsetX;
                        listTiles[indexCurrent].flowDirectionY = offsetY;
                    }
                }
            }
        }
    }
}

Vector2D Level::getFlowNormal(int x, int y) {
    int index = x + y * tileCountX;
    if (index > -1 && static_cast<size_t>(index) < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY)
        return Vector2D((float)listTiles[index].flowDirectionX, (float)listTiles[index].flowDirectionY).normalize();
    return Vector2D();
}

void Level::drawDecor(SDL_Renderer* renderer, float camX, float camY) {
    if (decorTextures.empty()) {
        std::cout << "⚠️ No decor textures to draw!" << std::endl;
        return;
    }
    for (size_t i = 0; i < decorPositions.size(); ++i) {
        // Tinh toa do pixel cua do trang tri, dieu chinh theo camera
        int xPos = (int)(decorPositions[i].x * 64) - (int)(camX * 64);
        int yPos = (int)(decorPositions[i].y * 64) - (int)(camY * 64);
        SDL_Rect decorRect = { xPos, yPos, 128, 128 };
        SDL_Texture* texture = decorTextures[i % decorTextures.size()];
        SDL_RenderCopy(renderer, texture, NULL, &decorRect);
    }
}

void Level::drawPotions(SDL_Renderer* renderer, float camX, float camY) {
    for (size_t i = 0; i < potionPositions.size(); i++) {
        int xPos = (int)(potionPositions[i].x * 64 - camX * 64);
        int yPos = (int)(potionPositions[i].y * 64 - camY * 64);
        SDL_Rect potionRect = { xPos, yPos, 32, 32 };

        // Hien thi texture tuy loai potion
        if (i % 2 == 0)
            SDL_RenderCopy(renderer, potionHealthTexture, NULL, &potionRect);
        else
            SDL_RenderCopy(renderer, potionManaTexture, NULL, &potionRect);
    }
}

void Level::checkPotionPickup(Vector2D characterPosition, Player* player) {
    AudioManager::init();
    for (auto it = potionPositions.begin(); it != potionPositions.end();) {
        Vector2D potionPos = *it;

        // Kiem tra khoang cach giua nhan vat va potion
        if ((potionPos - characterPosition).magnitude() < 1.0f) { // 1.0f la khoang cach va cham
            // Xac dinh loai potion
            size_t index = std::distance(potionPositions.begin(), it);
            if (potionTextures[index] == potionHealthTexture) {
                player->increaseHealth(); // Tang mau
                AudioManager::playSound("Data/Sound/heal.wav");
                Mix_VolumeChunk(AudioManager::getSound("Data/Sound/heal.wav"), 50);
            } else if (potionTextures[index] == potionManaTexture) {
                player->currentMP = player->maxMP; // Hoi phuc mana
                AudioManager::playSound("Data/Sound/levelup.wav");
                Mix_VolumeChunk(AudioManager::getSound("Data/Sound/levelup.wav"), 50);
            }
            // Xoa potion khoi danh sach
            it = potionPositions.erase(it);
            potionTextures.erase(potionTextures.begin() + index);
        } else {
            ++it;
        }
    }
}
