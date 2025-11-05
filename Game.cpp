#include "Game.h"
#include <iostream>
#include "HUD.h"
#include "Enemies/Orc.h"
#include "Enemies/Vampire.h"
#include "Enemies/Boss.h"
#include <SDL_ttf.h>
#include "Support/AudioManager.h"
#include "algorithm"



Game::Game(SDL_Window* window, SDL_Renderer* renderer, int windowWidth, int windowHeight)
    : level(renderer, 30, 20), spawnTimer(0.25f), roundTimer(5.0f), renderer_(renderer),
      windowWidth(windowWidth), windowHeight(windowHeight) {
    AudioManager::init();
    player = new Player(this, renderer, Vector2D(15, 10));
    hud = new HUD(renderer, player);

    SDL_Texture* fire = TextureLoader::loadTexture(renderer, "Fire.jpg");
    SDL_Texture* ice = TextureLoader::loadTexture(renderer, "Ice.jpg");
    hud->addSkill(fire, 5.0f);
    hud->addSkill(ice, 10.0f);
}

void Game::run() {
    auto time1 = std::chrono::system_clock::now();
    auto time2 = std::chrono::system_clock::now();
    const float dT = 1.0f / 60.0f;
    bool running = true;

    while (running) {
        time2 = std::chrono::system_clock::now();
        std::chrono::duration<float> timeDelta = time2 - time1;
        float timeDeltaFloat = timeDelta.count();

        if (timeDeltaFloat >= dT) {
            time1 = time2;
            std::cout << "Game::run - dT: " << dT << "\n";
            processEvents(renderer_, running);
            update(renderer_, dT, level);
            draw(renderer_);
            if (gameState == GameState::Quit) {
                running = false;
            }
        }
    }
}
Game::~Game() {
    delete player;
    delete hud;
    TextureLoader::deallocateTextures();
}

void Game::processEvents(SDL_Renderer* renderer, bool& running) {
    bool mouseDownThisFrame = false;
    AudioManager::init();
    //Process events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        }

        // <<< MỚI THÊM >>>
        // Không xử lý input game nếu đang ở Shop
        if (gameState == GameState::Shop) {
            continue;
        }

        // Goi handleInput cua HUD de xu ly thanh truot am luong
        if (hud->handleInput(event, this)) {
            continue; // Neu dang keo slider, khong xu ly input khac
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);

        // <<< MỚI THÊM >>>
        // Nhấn 'B' để mở Shop
        if (keyState[SDL_SCANCODE_B]) {
            if (gameState == GameState::Gameplay) { // Chỉ mở shop khi đang chơi
                setState(GameState::Shop); // Chuyển trạng thái sang Shop
                continue; // Dừng xử lý input khác
            }
        }

        // Ki nang Lua (Q)
        if (keyState[SDL_SCANCODE_Q]) {
            if (hud->skills[0].ready() && player->currentMP >= 10) {
            AudioManager::playSound("Data/Sound/fire.wav");
            Mix_VolumeChunk(AudioManager::getSound("Data/Sound/fire.wav"), 50); // 32 la am luong nho

                hud->useSkill(0); // Kich hoat ki nang
                // Offset dua tren huong cuoi cung cua nhan vat
                Vector2D offset = (player->getLastDirection() == 1) ? Vector2D(2.0f, -0.5f) : Vector2D(-3.0f, -0.5f);
                Vector2D firePos = player->getPos() + offset; // Cach nhan vat 1 don vi theo huong
                FireEffect fireEffect(renderer, firePos);
                addFireEffect(fireEffect); // Them hieu ung lua
                player->currentMP -= 10; // Tru MP sau khi da kiem tra
            }

        }

        // Ki nang Bang (E)
        if (keyState[SDL_SCANCODE_E]) {
            if (hud->skills[1].ready() && player->currentMP >= 15) { // Ki nang bang la skill thu 2
                AudioManager::playSound("Data/Sound/ice.wav");
                Mix_VolumeChunk(AudioManager::getSound("Data/Sound/ice.wav"), 50); // 32 la am luong nho

                hud->useSkill(1);
                Vector2D icePos = player->getPos() + Vector2D(0.0f, 1.0f);
                IceEffect iceEffect(renderer, icePos);
                addIceEffect(iceEffect);
                player->currentMP -= 15; // Tru MP sau khi dã kiem tra
            }
        }
    player->handleInput(keyState, renderer);
    }
}



void Game::update(SDL_Renderer* renderer, float dT, Level& level) {
    switch (gameState) {
        case GameState::Gameplay:
            if (player->isDead) {
                player->deathTimer.countDown(dT);
                player->frameTimer += dT;
                if (player->frameTimer >= player->frameTime) {
                    player->frameTimer = 0.0f;
                    if (player->frame < 6) {
                        player->frame++;
                    } else {
                        gameState = GameState::GameOver;
                    }
                }
                return;
            }

            updateUnits(dT);
            for (auto& coin : coins) coin->update(dT);
            updateFireEffects(dT);
            updateIceEffects(dT);
            updateSpawnUnitsIfRequired(renderer, dT);
            player->update(dT, listUnits, renderer, level);
            hud->update(dT, listUnits);
            level.checkPotionPickup(player->getPos(), player);
            updateCamera();

            if (!bossSpawned && spawnUnitCount == 0 && allEnemiesDead()) {
                std::cout << "Tat ca quai da chet! Spawn boss...\n";
                AudioManager::init();
                AudioManager::playSound("Data/Sound/warning.mp3");

                spawnBoss();
            }

            // Kiem tra Boss chet
            if (bossSpawned && allEnemiesDead()) {
                std::cout << "Boss da chet! Pha dao Map 1.\n";
                gameState = GameState::Victory; // Chuyen sang trang thai Victory
            }
            break;

        case GameState::GameOver:
            showGameOverMenu(renderer);
            break;

        case GameState::Menu:
            if (showMenu(renderer)) {
                gameState = GameState::Gameplay;
            } else {
                gameState = GameState::Quit;
            }
            break;

        case GameState::Victory:
            showVictoryMenu(renderer); // Hien thi khung Victory
            break;

        case GameState::Paused:
            showPauseMenu(renderer); // Hien thi Pause Menu
            break;

        // <<< MỚI THÊM >>>
        // Thêm case cho Shop
        case GameState::Shop:
            showShopMenu(renderer); // Goi hàm hien thi shop
            break;

        case GameState::Quit:
            break;
    }
}





void Game::updateUnits(float dT) {
    // Cap nhat truoc
    for (auto& unit : listUnits) {
        if (unit) {
            unit->update(dT, level, listUnits, *player);
        }
    }
    // Xoa sau
    listUnits.erase(
        std::remove_if(listUnits.begin(), listUnits.end(),
            [](const std::shared_ptr<Unit>& unit) { return !unit || unit->isDead(); }),
        listUnits.end()
    );
}




void Game::updateSpawnUnitsIfRequired(SDL_Renderer* renderer, float dT) {
    static bool roundStarted = false; // Bien de theo doi trang thai vong dau

    // Neu day la vong dau tien, bat dau ngay lap tuc
    if (!roundStarted) {
        spawnUnitCount = 15;  // Chi spawn 15 quai duy nhat
        roundStarted = true;
    }

    // Chi spawn quai neu con so luong can spawn va du thoi gian cho
    spawnTimer.countDown(dT);
    if (spawnUnitCount > 0 && spawnTimer.timeSIsZero()) {
        spawnTimer.resetToMax(); // Dat lai thoi gian spawn quai tiep theo

        // Chon 1 vi tri random tu 4 phia man hinh
        std::vector<Vector2D> spawnLocations = {
            {rand() % level.GetX(), 0},                         // Tren
            {rand() % level.GetX(), level.GetY() - 1},         // Duoi
            {0, rand() % level.GetY()},                         // Trai
            {level.GetX() - 1, rand() % level.GetY()}         // Phai
        };

        Vector2D spawnPos = spawnLocations[rand() % spawnLocations.size()];
        addUnit(renderer, spawnPos);
        spawnUnitCount--; // Giam so luong quai can spawn

        }
    }





void Game::draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // Mau xanh la cay giong co
    SDL_RenderClear(renderer); // Xoa sach renderer

    level.draw(renderer, tileSize, cameraPos.x, cameraPos.y);
    level.drawDecor(renderer, cameraPos.x, cameraPos.y);

    for (auto& unitSelected : listUnits)
        if (unitSelected) unitSelected->draw(renderer, tileSize, cameraPos);
    for (auto& fireEffect : listFireEffects)
        fireEffect.draw(renderer, tileSize, cameraPos);
    for (auto& iceEffect : listIceEffects)
        iceEffect.draw(renderer, tileSize, cameraPos);
    player->draw(renderer, tileSize, cameraPos);
    hud->draw(renderer);
    for (auto& coin : coins)
        coin->draw(renderer, tileSize, cameraPos);

    SDL_RenderPresent(renderer);
}


void Game::addUnit(SDL_Renderer* renderer, Vector2D posMouse) {
    int enemyType = rand() % 3;  // 0: Slime, 1: Orc

    if (enemyType == 0)
        listUnits.push_back(std::make_shared<Unit>(renderer, posMouse));  // Slime
    else if (enemyType == 1)
        listUnits.push_back(std::make_shared<Orc>(renderer, posMouse));   // Orc
    else
        listUnits.push_back(std::make_shared<Vampire>(renderer, posMouse));   // Orc

}



void Game::updateCamera() {
    Vector2D targetPos = player->getPos();
    std::cout<< player->getPos().x << " " << player->getPos().y;
    // Tinh vi tri muc tieu cua camera
    Vector2D targetCameraPos;
    targetCameraPos.x = targetPos.x - (windowWidth / (2 * tileSize));
    targetCameraPos.y = targetPos.y - (windowHeight / (2 * tileSize));

    // Noi suy muot ma
    const float lerpSpeed = 0.1f;
    cameraPos.x = cameraPos.x + (targetCameraPos.x - cameraPos.x) * lerpSpeed;
    cameraPos.y = cameraPos.y + (targetCameraPos.y - cameraPos.y) * lerpSpeed;

    // Gioi han camera
    float maxCameraX = level.GetX() - (windowWidth / tileSize);
    float maxCameraY = level.GetY() - (windowHeight / tileSize);

    if (cameraPos.x < 0) cameraPos.x = 0;
    if (cameraPos.y < 0) cameraPos.y = 0;
    if (cameraPos.x > maxCameraX) cameraPos.x = maxCameraX;
    if (cameraPos.y > maxCameraY) cameraPos.y = maxCameraY;

}


bool Game::allEnemiesDead() {
    for (auto& unit : listUnits) {
        if (unit->isAlive()) return false;
    }
    return true;
}




void Game::spawnBoss() {
    if (!renderer_) {
        std::cout << "Error: Renderer is null in Game::spawnBoss!" << std::endl;
        return;
    }

    std::cout << "Boss da xuat hien tai vi tri co dinh!\n";

    // Lay vi tri player
    Vector2D playerPos = player->getPos();
    std::cout << "Player Pos: (" << playerPos.x << ", " << playerPos.y << ")\n";

    // Chon huong ngau nhien hoac co dinh (vi du: ben phai player)
    Vector2D bossOffset(3.0f, 0.0f); // Cach player 3 don vi ben phai, ban co the thay doi
    // Mot so tuy chon khac:
    // Vector2D bossOffset(-3.0f, 0.0f); // Ben trai
    // Vector2D bossOffset(0.0f, -3.0f); // Phia tren
    // Vector2D bossOffset(0.0f, 3.0f);  // Phia duoi

    Vector2D bossPosition = playerPos + bossOffset;

    // Gioi han vi tri trong ban do
    float maxX = static_cast<float>(level.GetX() - 1); // tileCountX - 1
    float maxY = static_cast<float>(level.GetY() - 1); // tileCountY - 1
    bossPosition.x = std::max(0.5f, std::min(bossPosition.x, maxX - 0.5f));
    bossPosition.y = std::max(0.5f, std::min(bossPosition.y, maxY - 0.5f));

    // Tao boss
    auto boss = std::make_shared<Boss>(renderer_, bossPosition);
    if (!boss) {
        std::cout << "Error: Failed to create Boss!" << std::endl;
        return;
    }

    // Them vao danh sach don vi
    listUnits.push_back(boss);
    bossSpawned = true;

    // Debug thong tin
    std::cout << "Boss HP: " << boss->getHealth()
              << ", Pos: (" << bossPosition.x << ", " << bossPosition.y
              << "), List size: " << listUnits.size() << "\n";
}
void Game::updateFireEffects(float dT) {
    auto it = listFireEffects.begin();
    while (it != listFireEffects.end()) {
        it->update(dT, listUnits);
        if (it->isFinished()) {
            it = listFireEffects.erase(it);
        } else {
            ++it;
        }
    }
}


void Game::updateIceEffects(float dT) {
    auto it = listIceEffects.begin();
    while (it != listIceEffects.end()) {
        it->update(dT, listUnits);
        if (it->isFinished()) {
            it = listIceEffects.erase(it);
        } else {
            ++it;
        }
    }
}


bool Game::showMenu(SDL_Renderer* renderer) {
    bool inMenu = true;
    AudioManager::init();
    AudioManager::playMusic("Data/Sound/menu_background_music.ogg", -1);
    Mix_VolumeMusic(30);

    bool playSelected = false;

    SDL_Texture* background = TextureLoader::loadTexture(renderer, "menu_background.png");
    SDL_Texture* playButton = TextureLoader::loadTexture(renderer, "play_button.png");
    SDL_Texture* playButtonHover = TextureLoader::loadTexture(renderer, "play03.png");
    SDL_Texture* aboutButton = TextureLoader::loadTexture(renderer, "about_button.png");
    SDL_Texture* aboutButtonHover = TextureLoader::loadTexture(renderer, "about03.png");
    SDL_Texture* storeButton = TextureLoader::loadTexture(renderer, "back_button.png");
    SDL_Texture* storeButtonHover = TextureLoader::loadTexture(renderer, "back03.png");

    SDL_Rect playRect = { 100, 240, 150, 60 };
    SDL_Rect aboutRect = { 350, 240, 150, 60 };
    SDL_Rect storeRect = { 600, 240, 150, 60 };

    while (inMenu) {
        SDL_Event event;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        bool isHoverPlay = (mouseX >= playRect.x && mouseX <= playRect.x + playRect.w &&
                            mouseY >= playRect.y && mouseY <= playRect.y + playRect.h);
        bool isHoverAbout = (mouseX >= aboutRect.x && mouseX <= aboutRect.x + aboutRect.w &&
                             mouseY >= aboutRect.y && mouseY <= aboutRect.y + aboutRect.h);
        bool isHoverStore = (mouseX >= storeRect.x && mouseX <= storeRect.x + storeRect.w &&
                             mouseY >= storeRect.y && mouseY <= storeRect.y + storeRect.h);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameState = GameState::Quit;
                inMenu = false;
                playSelected = false;
                break;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (isHoverPlay) {
                    AudioManager::playSound("Data/Sound/press_button.mp3");
                    playSelected = true;
                    gameState = GameState::Gameplay;
                    inMenu = false;
                }
                if (isHoverAbout) {
                    AudioManager::playSound("Data/Sound/press_button.mp3");
                    if (showAboutScreen(renderer)) {
                        inMenu = false;
                    }
                }
                if (isHoverStore) {
                    AudioManager::playSound("Data/Sound/press_button.mp3");
                    gameState = GameState::Quit;
                    inMenu = false;              // Thoat vong lap menu
                    playSelected = false;        // Dam bao khong vao Gameplay
                    break;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, nullptr, nullptr);
        SDL_RenderCopy(renderer, isHoverPlay ? playButtonHover : playButton, nullptr, &playRect);
        SDL_RenderCopy(renderer, isHoverAbout ? aboutButtonHover : aboutButton, nullptr, &aboutRect);
        SDL_RenderCopy(renderer, isHoverStore ? storeButtonHover : storeButton, nullptr, &storeRect);
        SDL_RenderPresent(renderer);
    }

    if (playSelected) {
        Mix_HaltMusic();
        AudioManager::playMusic("Data/Sound/playing_background_music.mp3", -1);
        Mix_VolumeMusic(30);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(playButton);
    SDL_DestroyTexture(playButtonHover);
    SDL_DestroyTexture(aboutButton);
    SDL_DestroyTexture(aboutButtonHover);
    SDL_DestroyTexture(storeButton);
    SDL_DestroyTexture(storeButtonHover);

    return playSelected;
}

bool Game::showAboutScreen(SDL_Renderer* renderer) {
    bool inAbout = true;

    // Load texture voi kiem tra NULL
    SDL_Texture* background = TextureLoader::loadTexture(renderer, "menu_background.png");
    SDL_Texture* backButton = TextureLoader::loadTexture(renderer, "back_button.png");
    SDL_Texture* backButtonHover = TextureLoader::loadTexture(renderer, "back03.png");

    // Kiem tra load texture
    if (!background || !backButton) {
        std::cout << "Loi load texture: menu_background.png hoac back_button.png\n";
        return false;
    }

    // Tao overlay
    SDL_Texture* overlay = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);
    if (!overlay) {
        std::cout << "Loi tao overlay texture\n";
        SDL_DestroyTexture(background);
        SDL_DestroyTexture(backButton);
        return false;
    }

    SDL_SetTextureBlendMode(overlay, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, overlay);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);

    SDL_Rect backRect = { 20, 20, 100, 50 };

    while (inAbout) {
        SDL_Event event;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool isButtonHover = (mouseX >= backRect.x && mouseX <= backRect.x + backRect.w &&
                              mouseY >= backRect.y && mouseY <= backRect.y + backRect.h);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                inAbout = false;
                break;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (isButtonHover) {
                    AudioManager::playSound("Data/Sound/press_button.mp3");
                    inAbout = false;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, nullptr, nullptr);
        SDL_RenderCopy(renderer, overlay, nullptr, nullptr);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect infoBox = { 100, 100, 600, 400 };
        SDL_RenderFillRect(renderer, &infoBox);

        renderText(renderer, "NOT YOUR OWN ADVENTURE!", 150, 130, 32);
        renderText(renderer, "Developed: LXP", 150, 180, 24);
        renderText(renderer, "You play as a lone adventurer,", 150, 220, 24);
        renderText(renderer, "fighting enemies and surviving endless waves.", 150, 250, 24);
        renderText(renderer, "Controls:", 150, 290, 24);
        renderText(renderer, "- Move: Arrow Keys", 170, 320, 22);
        renderText(renderer, "- Attack: K", 170, 345, 22);
        renderText(renderer, "- Skill Q/E: Fire / Ice Attack", 170, 370, 22);
        renderText(renderer, "- ESC: Pause or Back", 170, 395, 22);
        renderText(renderer, "Press Back to return to the main menu", 150, 440, 22);

        SDL_RenderCopy(renderer, isButtonHover ? backButtonHover : backButton, nullptr, &backRect);
        SDL_RenderPresent(renderer);
    }

    // Don dep
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(overlay);
    SDL_DestroyTexture(backButton);
    return false;
}

void Game::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int fontSize) {
    // Khoi tao SDL_ttf
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cout << "SDL_ttf Init Error: " << TTF_GetError() << "\n";
            return;
        }
    }

    // Mo font
    TTF_Font* font = TTF_OpenFont("Data/Font/ThaleahFat.ttf", fontSize);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << "\n";
        return;
    }

    // Tao texture tu text
    SDL_Color color = { 0, 0, 0 }; // Mau den
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!textSurface) {
        std::cout << "Text Surface Error: " << TTF_GetError() << "\n";
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cout << "Text Texture Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return;
    }

    // Xac dinh vi tri hien thi
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };

    // Ve len renderer
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);

    // Giai phong bo nho
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void Game::showGameOverMenu(SDL_Renderer* renderer) {
    bool selecting = true;
    AudioManager::init();
    SDL_Event event;

    SDL_Texture* menuBackground = TextureLoader::loadTexture(renderer, "cut_frame_fixed.png");
    SDL_Texture* restartTexture = TextureLoader::loadTexture(renderer, "restart01.png");
    SDL_Texture* quitTexture = TextureLoader::loadTexture(renderer, "back_button.png");
    SDL_Texture* restartHoverTexture = TextureLoader::loadTexture(renderer, "restart03.png");
    SDL_Texture* quitHoverTexture = TextureLoader::loadTexture(renderer, "back03.png");

    SDL_Rect menuRect = { windowWidth / 2 - 200, windowHeight / 2 - 120, 400, 120 };
    SDL_Rect retryButton = { windowWidth / 2 - 150, windowHeight / 2 - 50, 100, 40 };
    SDL_Rect quitButton = { windowWidth / 2 + 55, windowHeight / 2 - 50, 100, 40 };

    while (selecting) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool isRestart = (mouseX >= retryButton.x && mouseX <= retryButton.x + retryButton.w &&
                          mouseY >= retryButton.y && mouseY <= retryButton.y + retryButton.h);
        bool isQuit = (mouseX >= quitButton.x && mouseX <= quitButton.x + quitButton.w &&
                       mouseY >= quitButton.y && mouseY <= quitButton.y + quitButton.h);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameState = GameState::Quit;
                    selecting = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (isRestart) {
                        AudioManager::playSound("Data/Sound/press_button.mp3");
                        restartGame();
                        gameState = GameState::Gameplay;
                        selecting = false;
                    }
                    if (isQuit) {
                        AudioManager::playSound("Data/Sound/press_button.mp3");
                        restartGame();
                        if (showMenu(renderer)) {
                            gameState = GameState::Gameplay;
                        } else {
                            gameState = GameState::Quit;
                        }
                        selecting = false;
                    }
                    break;
            }
        }

        // Ve lai khung canh gameplay thay vi xoa man hinh
        level.draw(renderer, tileSize, cameraPos.x, cameraPos.y);
        for (auto& unitSelected : listUnits)
            if (unitSelected != nullptr)
                unitSelected->draw(renderer, tileSize, cameraPos);
        for (auto& fireEffect : listFireEffects)
            fireEffect.draw(renderer, tileSize, cameraPos);
        for (auto& iceEffect : listIceEffects)
            iceEffect.draw(renderer, tileSize, cameraPos);
        player->draw(renderer, tileSize, cameraPos);
        level.drawDecor(renderer, cameraPos.x, cameraPos.y);
        hud->draw(renderer);
        for (auto& coin : coins)
            coin->draw(renderer, tileSize, cameraPos);

        // Ve menu Game Over len tren
        SDL_RenderCopy(renderer, menuBackground, nullptr, &menuRect);
        renderText(renderer, "YOU LOST!", windowWidth / 2 - 50, windowHeight / 2 - 80, 30);
        SDL_RenderCopy(renderer, isRestart ? restartHoverTexture : restartTexture, nullptr, &retryButton);
        SDL_RenderCopy(renderer, isQuit ? quitHoverTexture : quitTexture, nullptr, &quitButton);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(menuBackground);
    SDL_DestroyTexture(restartTexture);
    SDL_DestroyTexture(quitTexture);
    SDL_DestroyTexture(restartHoverTexture);
    SDL_DestroyTexture(quitHoverTexture);
}

void Game::restartGame() {
    player->reset();
    listUnits.clear();
    listFireEffects.clear();
    listIceEffects.clear();
    coins.clear();
    bossSpawned = false;
    spawnUnitCount = 15;
    gameOver = false;
    gameState = GameState::Gameplay;
    hud->survivalTime = 0.0f;
}

void Game::showVictoryMenu(SDL_Renderer* renderer) {
    bool selecting = true;
    SDL_Event event;

    SDL_Texture* victoryBackground = TextureLoader::loadTexture(renderer, "cut_frame_fixed.png"); // Gia dinh co file anh
    SDL_Texture* menuButton = TextureLoader::loadTexture(renderer, "back_button.png");
    SDL_Texture* menuButtonHover = TextureLoader::loadTexture(renderer, "back03.png");

    SDL_Rect victoryRect = { windowWidth / 2 - 200, windowHeight / 2 - 120, 400, 200 };
    SDL_Rect menuRect = { windowWidth / 2 - 50, windowHeight / 2 - 10, 100, 40 };

    while (selecting) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool isMenuHover = (mouseX >= menuRect.x && mouseX <= menuRect.x + menuRect.w &&
                            mouseY >= menuRect.y && mouseY <= menuRect.y + menuRect.h);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameState = GameState::Quit;
                    selecting = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (isMenuHover) {
                        AudioManager::playSound("Data/Sound/press_button.mp3");
                        restartGame(); // Reset game truoc khi ve menu
                        if (showMenu(renderer)) {
                            gameState = GameState::Gameplay;
                        } else {
                            gameState = GameState::Quit;
                        }
                        selecting = false;
                    }
                    break;
            }
        }

        // Ve lai khung canh gameplay lam nen
        level.draw(renderer, tileSize, cameraPos.x, cameraPos.y);
        for (auto& unitSelected : listUnits)
            if (unitSelected != nullptr)
                unitSelected->draw(renderer, tileSize, cameraPos);
        for (auto& fireEffect : listFireEffects)
            fireEffect.draw(renderer, tileSize, cameraPos);
        for (auto& iceEffect : listIceEffects)
            iceEffect.draw(renderer, tileSize, cameraPos);
        player->draw(renderer, tileSize, cameraPos);
        level.drawDecor(renderer, cameraPos.x, cameraPos.y);
        hud->draw(renderer);
        for (auto& coin : coins)
            coin->draw(renderer, tileSize, cameraPos);

        // Ve khung Victory
        SDL_RenderCopy(renderer, victoryBackground, nullptr, &victoryRect);
        renderText(renderer, "Map 1 cleared!", windowWidth / 2 - 80, windowHeight / 2 - 80, 30);
        renderText(renderer, "New map is updating...", windowWidth / 2 - 80, windowHeight / 2 - 50, 20);
        SDL_RenderCopy(renderer, isMenuHover ? menuButtonHover : menuButton, nullptr, &menuRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(victoryBackground);
    SDL_DestroyTexture(menuButton);
    SDL_DestroyTexture(menuButtonHover);
}

void Game::showPauseMenu(SDL_Renderer* renderer) {
    bool paused = true;
    SDL_Event event;

    SDL_Texture* pauseBackground = TextureLoader::loadTexture(renderer, "cut_frame_fixed.png"); // Gia dinh file anh
    SDL_Texture* resumeButton = TextureLoader::loadTexture(renderer, "back_button.png");
    SDL_Texture* resumeButtonHover = TextureLoader::loadTexture(renderer, "back03.png");

    SDL_Rect pauseRect = { windowWidth / 2 - 200, windowHeight / 2 - 120, 400, 150 };
    SDL_Rect resumeRect = { windowWidth / 2 - 50, windowHeight / 2 - 30, 100, 40 };

    while (paused) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool isResumeHover = (mouseX >= resumeRect.x && mouseX <= resumeRect.x + resumeRect.w &&
                              mouseY >= resumeRect.y && mouseY <= resumeRect.y + resumeRect.h);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    gameState = GameState::Quit;
                    paused = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (isResumeHover) {
                        AudioManager::playSound("Data/Sound/press_button.mp3");
                        gameState = GameState::Gameplay; // Tiep tuc game
                        paused = false;
                    }
                    break;
            }
        }

        // Ve lai khung canh gameplay lam nen
        level.draw(renderer, tileSize, cameraPos.x, cameraPos.y);
        for (auto& unitSelected : listUnits)
            if (unitSelected != nullptr)
                unitSelected->draw(renderer, tileSize, cameraPos);
        for (auto& fireEffect : listFireEffects)
            fireEffect.draw(renderer, tileSize, cameraPos);
        for (auto& iceEffect : listIceEffects)
            iceEffect.draw(renderer, tileSize, cameraPos);
        player->draw(renderer, tileSize, cameraPos);
        level.drawDecor(renderer, cameraPos.x, cameraPos.y);
        hud->draw(renderer);
        for (auto& coin : coins)
            coin->draw(renderer, tileSize, cameraPos);

        // Ve khung Pause
        SDL_RenderCopy(renderer, pauseBackground, nullptr, &pauseRect);
        renderText(renderer, "Game Paused", windowWidth / 2 - 75, windowHeight / 2 - 70, 30);
        SDL_RenderCopy(renderer, isResumeHover ? resumeButtonHover : resumeButton, nullptr, &resumeRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(pauseBackground);
    SDL_DestroyTexture(resumeButton);
    SDL_DestroyTexture(resumeButtonHover);
}

void Game::showShopMenu(SDL_Renderer* renderer) {
    bool inShop = true;
    SDL_Event event;

    // Tai tai nguyen cho Shop
    SDL_Texture* shopBackground = TextureLoader::loadTexture(renderer, "cut_frame_fixed.png");
    SDL_Texture* buyButtonTexture = TextureLoader::loadTexture(renderer, "buy01.png");
    SDL_Texture* buyButtonHoverTexture = TextureLoader::loadTexture(renderer, "buy03.png");
    SDL_Texture* exitButtonTexture = TextureLoader::loadTexture(renderer, "back_button.png");
    SDL_Texture* exitButtonHoverTexture = TextureLoader::loadTexture(renderer, "back03.png");

    // Dinh vi cac nut va vat pham
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;

    SDL_Rect shopRect = { centerX - 250, centerY - 150, 500, 300 };

    // Item 1 (HP): Vị trí va kich thuoc item container
    SDL_Rect item1Rect = { shopRect.x + 30, shopRect.y + 100, 440, 40 };
    // Nut BUY 1
    SDL_Rect buyItem1Rect = { item1Rect.x + 350, item1Rect.y, 80, 40 };

    // Item 2 (ATK): Vị trí va kich thuoc item container
    SDL_Rect item2Rect = { shopRect.x + 30, shopRect.y + 180, 440, 40 };
    // Nut BUY 2
    SDL_Rect buyItem2Rect = { item2Rect.x + 350, item2Rect.y, 80, 40 };

    // Nut BACK
    SDL_Rect backRect = { shopRect.x + shopRect.w - 110, shopRect.y + shopRect.h - 50, 100, 40 };

    int item1Price = 10;
    int item2Price = 15;

    while (inShop) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Kiem tra hover
        bool isHoverBack = (mouseX >= backRect.x && mouseX <= backRect.x + backRect.w &&
                            mouseY >= backRect.y && mouseY <= backRect.y + backRect.h);

        bool isHoverBuy1 = (mouseX >= buyItem1Rect.x && mouseX <= buyItem1Rect.x + buyItem1Rect.w &&
                            mouseY >= buyItem1Rect.y && mouseY <= buyItem1Rect.y + buyItem1Rect.h);

        bool isHoverBuy2 = (mouseX >= buyItem2Rect.x && mouseX <= buyItem2Rect.x + buyItem2Rect.w &&
                            mouseY >= buyItem2Rect.y && mouseY <= buyItem2Rect.y + buyItem2Rect.h);


        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameState = GameState::Quit;
                inShop = false;
                break;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Xu ly click nut BACK
                if (isHoverBack) {
                    AudioManager::playSound("Data/Sound/press_button.mp3");
                    gameState = GameState::Gameplay;
                    inShop = false;
                    break;
                }

                // Logic mua Nang cap HP (Item 1)
                if (isHoverBuy1) {
                    if (player->getCoins() >= item1Price) {
                        player->upgradeHP(20);
                        player->coin -= item1Price;
                        AudioManager::playSound("Data/Sound/coin_pickup.mp3");
                    } else {
                        AudioManager::playSound("Data/Sound/warning.mp3");
                    }
                }

                // Logic mua Nang cap Sat thuong (Item 2)
                if (isHoverBuy2) {
                    if (player->getCoins() >= item2Price) {
                        player->upgradeDamage(5);
                        player->coin -= item2Price;
                        AudioManager::playSound("Data/Sound/coin_pickup.mp3");
                    } else {
                        AudioManager::playSound("Data/Sound/warning.mp3");
                    }
                }
            }
        }

        // Ve khung nen shop
        SDL_RenderCopy(renderer, shopBackground, nullptr, &shopRect);

        // Tieu de
        renderText(renderer, "THE ARMORY SHOP", shopRect.x + 100, shopRect.y + 20, 32);

        // Coins hien tai
        renderText(renderer, "COINS:", shopRect.x + 20, shopRect.y + 60, 20);
        renderText(renderer, std::to_string(player->getCoins()), shopRect.x + 100, shopRect.y + 60, 20);

        // ITEM 1: HP
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &item1Rect);
        renderText(renderer, "MAX HP UPGRADE (+20)", item1Rect.x + 5, item1Rect.y + 5, 18);
        renderText(renderer, "COST: 10", item1Rect.x + 5, item1Rect.y + 25, 18);

        // Nut BUY 1
        SDL_RenderCopy(renderer, isHoverBuy1 ? buyButtonHoverTexture : buyButtonTexture, nullptr, &buyItem1Rect);
        renderText(renderer, "", buyItem1Rect.x + 25, buyItem1Rect.y + 10, 18);

        // ITEM 2: DAMAGE
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &item2Rect);
        renderText(renderer, "ATTACK DAMAGE (+5)", item2Rect.x + 5, item2Rect.y + 5, 18);
        renderText(renderer, "COST: 15", item2Rect.x + 5, item2Rect.y + 25, 18);

        // Nut BUY 2
        SDL_RenderCopy(renderer, isHoverBuy2 ? buyButtonHoverTexture : buyButtonTexture, nullptr, &buyItem2Rect);
        renderText(renderer, "", buyItem2Rect.x + 25, buyItem2Rect.y + 10, 18);

        // Nut BACK
        SDL_RenderCopy(renderer, isHoverBack ? exitButtonHoverTexture : exitButtonTexture, nullptr, &backRect);
        renderText(renderer, "", backRect.x + 25, backRect.y + 10, 18);

        SDL_RenderPresent(renderer);
    }

    // Huy tai nguyen
    SDL_DestroyTexture(shopBackground);
    SDL_DestroyTexture(buyButtonTexture);
    SDL_DestroyTexture(buyButtonHoverTexture);
    SDL_DestroyTexture(exitButtonTexture);
    SDL_DestroyTexture(exitButtonHoverTexture);
}
