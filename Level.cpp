#include "Level.h"
#include <iostream>
#include "Player.h"
#include "Support/AudioManager.h"
<<<<<<< HEAD

=======
#include <queue>
#include <algorithm>
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)

Level::Level(SDL_Renderer* renderer, int setTileCountX, int setTileCountY)
    : tileCountX(setTileCountX), tileCountY(setTileCountY),
      targetX(setTileCountX / 2), targetY(setTileCountY / 2) {
    lastPotionSpawnTime = std::chrono::steady_clock::now() - std::chrono::seconds(15);
    textureGrass = TextureLoader::loadTexture(renderer, "grass.jpg");
    potionHealthTexture = TextureLoader::loadTexture(renderer, "potion.png");
    potionManaTexture = TextureLoader::loadTexture(renderer, "Health_potion.png");
<<<<<<< HEAD
    // Load đồ vật trang trí
=======

    // Load do vat trang tri
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
    decorTextures.push_back(TextureLoader::loadTexture(renderer, "Autumn_tree1.png"));
    decorTextures.push_back(TextureLoader::loadTexture(renderer, "Autumn_tree1.png"));
    decorTextures.push_back(TextureLoader::loadTexture(renderer, "Fruit_tree1.png"));

<<<<<<< HEAD
    // Thêm dòng này để tạo vị trí cho vật trang trí
    generateDecorPositions();

    // Thêm địch vào các góc
=======
    // Them dong nay de tao vi tri cho vat trang tri
    generateDecorPositions();

    // Them dich vao cac goc
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
    int xMax = tileCountX - 1;
    int yMax = tileCountY - 1;
    setTileType(0, 0, TileType::enemySpawner);
    setTileType(xMax, 0, TileType::enemySpawner);
    setTileType(0, yMax, TileType::enemySpawner);
    setTileType(xMax, yMax, TileType::enemySpawner);

    calculateFlowField();
<<<<<<< HEAD

    spawnPotions();
}


void Level::generateDecorPositions() {
    decorPositions.clear();
    int numDecor = 7;  // Số lượng đồ trang trí
    float minSpacing = 3.0f; //  Khoảng cách tối thiểu giữa các đồ trang trí (đơn vị tile)
=======
    spawnPotions();
}

void Level::generateDecorPositions() {
    decorPositions.clear();
    // So luong do trang tri
    int numDecor = 7;
    // Khoang cach toi thieu giua cac do trang tri (don vi tile)
    float minSpacing = 3.0f;
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)

    for (int i = 0; i < numDecor; i++) {
        bool validPosition = false;
        Vector2D newDecor;

<<<<<<< HEAD
        //  Lặp lại cho đến khi tìm được vị trí hợp lệ
=======
        // Lap lai cho den khi tim duoc vi tri hop le
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        while (!validPosition) {
            int randX = rand() % tileCountX;
            int randY = rand() % tileCountY;
            newDecor = Vector2D(randX, randY);
            validPosition = true;

<<<<<<< HEAD
            //  Kiểm tra khoảng cách với các vật trang trí đã đặt
            for ( auto& decor : decorPositions) {
=======
            // Kiem tra khoang cach voi cac vat trang tri da dat
            for (auto& decor : decorPositions) {
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
                if ((decor - newDecor).magnitude() < minSpacing) {
                    validPosition = false;
                    break;
                }
            }
        }
<<<<<<< HEAD

=======
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        decorPositions.push_back(newDecor);
    }
}

<<<<<<< HEAD


=======
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
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
<<<<<<< HEAD
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Màu xanh lá cây làm nền tạm thời
=======
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        SDL_RenderFillRect(renderer, nullptr);
    } else {
        for (int y = 0; y < tileCountY; y++) {
            for (int x = 0; x < tileCountX; x++) {
<<<<<<< HEAD
                // Sử dụng std::round để căn chỉnh chính xác
                int xPos = std::round(x * tileSize - camX);
                int yPos = std::round(y * tileSize - camY);
                // Tăng kích thước tile thêm 1 pixel để che khe hở
=======
                // Su dung std::round de can chinh chinh xac
                int xPos = std::round(x * tileSize - camX);
                int yPos = std::round(y * tileSize - camY);
                // Tang kich thuoc tile them 1 pixel de che khe ho
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
                SDL_Rect rect = {xPos, yPos, tileSize + 1, tileSize + 1};
                SDL_RenderCopy(renderer, textureGrass, NULL, &rect);
            }
        }
    }
<<<<<<< HEAD

    // Vẽ potion
    drawPotions(renderer, camX, camY);
}


=======
    // Ve potion
    drawPotions(renderer, camX, camY);
}

>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
void Level::spawnPotions() {
    int numPotions = 2;
    float minSpacing = 3.0f;
    int maxAttempts = 100;

    for (int i = 0; i < numPotions; i++) {
        bool validPosition = false;
<<<<<<< HEAD
        Vector2D newPotion;
=======
        Vector2D newPotionPos;
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)

        int attempts = 0;
        while (!validPosition && attempts < maxAttempts) {
            int randX = rand() % tileCountX;
            int randY = rand() % tileCountY;
<<<<<<< HEAD
            newPotion = Vector2D(randX, randY);
            validPosition = true;
            attempts++;

            // Kiểm tra vị trí không phải enemySpawner
=======
            newPotionPos = Vector2D(randX, randY);
            validPosition = true;
            attempts++;

            // Kiem tra vi tri khong phai enemySpawner
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
            if (getTileType(randX, randY) == TileType::enemySpawner) {
                validPosition = false;
                continue;
            }

<<<<<<< HEAD
            // Kiểm tra khoảng cách với các potion khác
            for (auto& potion : potionPositions) {
                if ((potion - newPotion).magnitude() < minSpacing) {
=======
            // Kiem tra khoang cach voi cac potion khac
            for (auto& potion : potionPositions) {
                if ((potion - newPotionPos).magnitude() < minSpacing) {
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
                    validPosition = false;
                    break;
                }
            }
<<<<<<< HEAD

            // Nếu tìm thấy vị trí hợp lệ
            if (validPosition) {
                potionPositions.push_back(newPotion);

                // Random loại potion
                SDL_Texture* potionTexture = (rand() % 2 == 0) ? potionHealthTexture : potionManaTexture;
                potionTextures.push_back(potionTexture);

                std::cout << "Potion spawned at: (" << newPotion.x << ", " << newPotion.y << ")\n";
=======
            // Neu tim thay vi tri hop le
            if (validPosition) {
                potionPositions.push_back(newPotionPos);
                // Random loai potion
                SDL_Texture* potionTexture = (rand() % 2 == 0) ? potionHealthTexture : potionManaTexture;
                potionTextures.push_back(potionTexture);
                std::cout << "Potion spawned at: (" << newPotionPos.x << ", " << newPotionPos.y << ")\n";
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
            }
        }
    }
}

<<<<<<< HEAD

Vector2D Level::getRandomEnemySpawnerLocation() {
// Tạo danh sách ô quái được spawn
    std::vector<int> listSpawnerIndices;
    for (int count = 0; count < listTiles.size(); count++) {
=======
Vector2D Level::getRandomEnemySpawnerLocation() {
    // Tao danh sach o quai duoc spawn
    std::vector<int> listSpawnerIndices;
    for (int count = 0; static_cast<size_t>(count) < listTiles.size(); count++) {
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        auto& tileSelected = listTiles[count];
        if (tileSelected.type == TileType::enemySpawner)
            listSpawnerIndices.push_back(count);
    }
<<<<<<< HEAD

// Nếu có 1 hoặc nhiều con đc spawn thì chọn ngẫu nhiên 1 con ở vị trí trung tâm ô gạch
    if (listSpawnerIndices.empty() == false) {
        int index = listSpawnerIndices[rand() % listSpawnerIndices.size()];
        return Vector2D((float)(index % tileCountX) + 0.5f, (float)(index / tileCountX) + 0.5f);
    }

    return Vector2D(0.5f, 0.5f);
}



Level::TileType Level::getTileType(int x, int y) {
    int index = x + y * tileCountX;
    if (index > -1 && index < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY)
        return listTiles[index].type;

=======
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
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
    return TileType::empty;
}

void Level::setTileType(int x, int y, TileType tileType) {
    int index = x + y * tileCountX;
<<<<<<< HEAD
    if (index > -1 && index < listTiles.size() &&
=======
    if (index > -1 && static_cast<size_t>(index) < listTiles.size() &&
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY) {
        listTiles[index].type = tileType;
        calculateFlowField();
    }
}

<<<<<<< HEAD



void Level::calculateFlowField() {
// Đảm bảo các con quái ở bounds
    int indexTarget = targetX + targetY * tileCountX;
    if (indexTarget > -1 && indexTarget < listTiles.size() &&
        targetX > -1 && targetX < tileCountX &&
        targetY > -1 && targetY < tileCountY) {

// Reset lại cái flow field
=======
void Level::calculateFlowField() {
    // Dam bao cac con quai o bounds
    int indexTarget = targetX + targetY * tileCountX;
    if (indexTarget > -1 && static_cast<size_t>(indexTarget) < listTiles.size() &&
        targetX > -1 && targetX < tileCountX &&
        targetY > -1 && targetY < tileCountY) {
        // Reset lai cai flow field
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        for (auto& tileSelected : listTiles) {
            tileSelected.flowDirectionX = 0;
            tileSelected.flowDirectionY = 0;
            tileSelected.flowDistance = flowDistanceMax;
        }
<<<<<<< HEAD
// Tính toán cái flow field
=======
        // Tinh toan cai flow field
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        calculateDistances();
        calculateFlowDirections();
    }
}

<<<<<<< HEAD

void Level::calculateDistances() {
    int indexTarget = targetX + targetY * tileCountX;

//Tạo một hàng đợi chứa các chỉ số cần được kiểm tra.
    std::queue<int> listIndicesToCheck;
//Đặt giá trị dòng chảy (flow) của ô mục tiêu thành 0 và thêm nó vào hàng đợi.
    listTiles[indexTarget].flowDistance = 0;
    listIndicesToCheck.push(indexTarget);

//Độ lệch của các ô lân cận cần được kiểm tra.
    const int listNeighbors[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

//Lặp qua hàng đợi và gán khoảng cách cho từng ô.
    while (listIndicesToCheck.empty() == false) {
        int indexCurrent = listIndicesToCheck.front();
        listIndicesToCheck.pop();

//Kiểm tra từng ô lân cận;
        for (int count = 0; count < 4; count++) {
            int neighborX = listNeighbors[count][0] + indexCurrent % tileCountX;
            int neighborY = listNeighbors[count][1] + indexCurrent / tileCountX;
            int indexNeighbor = neighborX + neighborY * tileCountX;

//Đảm bảo rằng ô lân cận tồn tại và không phải là tường.
            if (indexNeighbor > -1 && indexNeighbor < listTiles.size() &&
                neighborX > -1 && neighborX < tileCountX &&
                neighborY > -1 && neighborY < tileCountY &&
                listTiles[indexNeighbor].type != TileType::wall) {

//Kiểm tra xem ô đó đã được gán khoảng cách hay chưa.
                if (listTiles[indexNeighbor].flowDistance == flowDistanceMax) {
//Nếu chưa thì gán khoảng cách cho nó và thêm vào hàng đợi.
=======
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
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
                    listTiles[indexNeighbor].flowDistance = listTiles[indexCurrent].flowDistance + 1;
                    listIndicesToCheck.push(indexNeighbor);
                }
            }
        }
    }
}

<<<<<<< HEAD

void Level::calculateFlowDirections() {
//Độ lệch của các ô lân cận cần được kiểm tra.
    const int listNeighbors[][2] = {
        {-1, 0}, {-1, 1}, {0, 1}, {1, 1},
        {1, 0}, {1, -1}, {0, -1}, {-1, -1} };

    for (int indexCurrent = 0; indexCurrent < listTiles.size(); indexCurrent++) {
//Đảm bảo rằng ô đã được gán giá trị khoảng cách.
        if (listTiles[indexCurrent].flowDistance != flowDistanceMax) {
//Đặt khoảng cách tốt nhất bằng khoảng cách của ô hiện tại.
            unsigned char flowFieldBest = listTiles[indexCurrent].flowDistance;

            //Check each of the neighbors;
            for (int count = 0; count < 8; count++) {
                int offsetX = listNeighbors[count][0];
                int offsetY = listNeighbors[count][1];

                int neighborX = offsetX + indexCurrent % tileCountX;
                int neighborY = offsetY + indexCurrent / tileCountX;
                int indexNeighbor = neighborX + neighborY * tileCountX;

                //Đảm bảo lân cận tồn tại
                if (indexNeighbor > -1 && indexNeighbor < listTiles.size() &&
                    neighborX > -1 && neighborX < tileCountX &&
                    neighborY > -1 && neighborY < tileCountY) {
//Nếu khoảng cách của ô lân cận hiện tại thấp hơn khoảng cách tốt nhất, thì sử dụng nó.
=======
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
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
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

<<<<<<< HEAD


Vector2D Level::getFlowNormal(int x, int y) {
    int index = x + y * tileCountX;
    if (index > -1 && index < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY)
        return Vector2D((float)listTiles[index].flowDirectionX, (float)listTiles[index].flowDirectionY).normalize();

    return Vector2D();
}



=======
Vector2D Level::getFlowNormal(int x, int y) {
    int index = x + y * tileCountX;
    if (index > -1 && static_cast<size_t>(index) < listTiles.size() &&
        x > -1 && x < tileCountX &&
        y > -1 && y < tileCountY)
        return Vector2D((float)listTiles[index].flowDirectionX, (float)listTiles[index].flowDirectionY).normalize();
    return Vector2D();
}

>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
void Level::drawDecor(SDL_Renderer* renderer, float camX, float camY) {
    if (decorTextures.empty()) {
        std::cout << "⚠️ No decor textures to draw!" << std::endl;
        return;
    }
<<<<<<< HEAD

    for (size_t i = 0; i < decorPositions.size(); ++i) {
        // Tính tọa độ pixel của đồ trang trí, điều chỉnh theo camera
=======
    for (size_t i = 0; i < decorPositions.size(); ++i) {
        // Tinh toa do pixel cua do trang tri, dieu chinh theo camera
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        int xPos = (int)(decorPositions[i].x * 64) - (int)(camX * 64);
        int yPos = (int)(decorPositions[i].y * 64) - (int)(camY * 64);
        SDL_Rect decorRect = { xPos, yPos, 128, 128 };
        SDL_Texture* texture = decorTextures[i % decorTextures.size()];
<<<<<<< HEAD


=======
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        SDL_RenderCopy(renderer, texture, NULL, &decorRect);
    }
}

<<<<<<< HEAD

=======
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
void Level::drawPotions(SDL_Renderer* renderer, float camX, float camY) {
    for (size_t i = 0; i < potionPositions.size(); i++) {
        int xPos = (int)(potionPositions[i].x * 64 - camX * 64);
        int yPos = (int)(potionPositions[i].y * 64 - camY * 64);
        SDL_Rect potionRect = { xPos, yPos, 32, 32 };

<<<<<<< HEAD
        // Hiển thị texture tùy loại potion
=======
        // Hien thi texture tuy loai potion
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
        if (i % 2 == 0)
            SDL_RenderCopy(renderer, potionHealthTexture, NULL, &potionRect);
        else
            SDL_RenderCopy(renderer, potionManaTexture, NULL, &potionRect);
    }
}

<<<<<<< HEAD

=======
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
void Level::checkPotionPickup(Vector2D characterPosition, Player* player) {
    AudioManager::init();
    for (auto it = potionPositions.begin(); it != potionPositions.end();) {
        Vector2D potionPos = *it;

<<<<<<< HEAD
        // Kiểm tra khoảng cách giữa nhân vật và potion
        if ((potionPos - characterPosition).magnitude() < 1.0f) { // 1.0f là khoảng cách va chạm

            // Xác định loại potion
            size_t index = std::distance(potionPositions.begin(), it);
            if (potionTextures[index] == potionHealthTexture) {
                player->increaseHealth(); // Tăng máu
                AudioManager::playSound("Data/Sound/heal.wav");
                Mix_VolumeChunk(AudioManager::getSound("Data/Sound/heal.wav"), 50);


            } else if (potionTextures[index] == potionManaTexture) {
                player->levelUp(); // Tăng cấp
                AudioManager::playSound("Data/Sound/levelup.wav");
                Mix_VolumeChunk(AudioManager::getSound("Data/Sound/levelup.wav"), 50);
            }

            // Xóa potion khỏi danh sách
=======
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
>>>>>>> 173afb8 (Sửa lỗi tự trừ hp)
            it = potionPositions.erase(it);
            potionTextures.erase(potionTextures.begin() + index);
        } else {
            ++it;
        }
    }
}
