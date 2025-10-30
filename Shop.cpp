#include "Shop.h"
#include <iostream>
#include <SDL_ttf.h>
#include "../Game.h"

// Implement renderText here if it's not globally available,
// or ensure Game::renderText is accessible (e.g., make it static or a utility function).
// For simplicity, I'll copy the renderText implementation from Game.cpp here,
// assuming it's a static/utility function or you move it to a common utility file.

// NOTE: Duoc trien khai giong Game::renderText trong Game.cpp
void Shop::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int fontSize) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cout << "SDL_ttf Init Error: " << TTF_GetError() << "\n";
            return;
        }
    }
    TTF_Font* font = TTF_OpenFont("Data/Font/ThaleahFat.ttf", fontSize);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << "\n";
        return;
    }
    SDL_Color color = { 0, 0, 0 }; // Mau den
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!textSurface) {
        TTF_CloseFont(font);
        return;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return;
    }

    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}
// End of renderText implementation

Shop::Shop(SDL_Renderer* renderer, Player* playerPtr, Game* gamePtr, int wWidth, int wHeight)
    : player(playerPtr), game(gamePtr), renderer_(renderer),
      windowWidth(wWidth), windowHeight(wHeight) {

    // Load Textures
    menuBackground = TextureLoader::loadTexture(renderer, "cut_frame_fixed.png");
    buyButton = TextureLoader::loadTexture(renderer, "play_button.png");
    buyButtonHover = TextureLoader::loadTexture(renderer, "play03.png");
    backButton = TextureLoader::loadTexture(renderer, "back_button.png");
    backButtonHover = TextureLoader::loadTexture(renderer, "back03.png");

    // Initialize Rects
    storeRect = { windowWidth / 2 - 200, windowHeight / 2 - 150, 400, 300 };
    buyRect = { windowWidth / 2 - 150, windowHeight / 2 + 50, 100, 40 };
    backRect = { windowWidth / 2 + 50, windowHeight / 2 + 50, 100, 40 };
}

Shop::~Shop() {
    // Textures loaded by TextureLoader should be deallocated via TextureLoader::deallocateTextures()
    // or manually if loaded outside (assuming they are managed centrally, so we skip manual destroy here).
}

bool Shop::handleInput(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        bool isHoverBuy = (mouseX >= buyRect.x && mouseX <= buyRect.x + buyRect.w &&
                           mouseY >= buyRect.y && mouseY <= buyRect.y + buyRect.h);
        bool isHoverBack = (mouseX >= backRect.x && mouseX <= backRect.x + backRect.w &&
                            mouseY >= backRect.y && mouseY <= backRect.y + backRect.h);

        if (isHoverBuy) {
            AudioManager::playSound("Data/Sound/press_button.mp3");
            if (player->spendCoins(ITEM_PRICE)) {
                player->addResurrectionItem();
                std::cout << "Da mua 1 Item Hoi Sinh. So luong: " << player->getResurrectionCount() << "\n";
            } else {
                std::cout << "Khong du xu! Can " << ITEM_PRICE << " xu.\n";
            }
            return true;
        }

        if (isHoverBack) {
            AudioManager::playSound("Data/Sound/press_button.mp3");
            game->setState(GameState::Menu);
            return true;
        }
    }
    return false;
}

void Shop::draw(SDL_Renderer* renderer) {
    if (!renderer) return;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    bool isHoverBuy = (mouseX >= buyRect.x && mouseX <= buyRect.x + buyRect.w &&
                       mouseY >= buyRect.y && mouseY <= buyRect.y + buyRect.h);
    bool isHoverBack = (mouseX >= backRect.x && mouseX <= backRect.x + backRect.w &&
                        mouseY >= backRect.y && mouseY <= backRect.y + backRect.h);

    // 1. Ve khung Shop
    SDL_RenderCopy(renderer, menuBackground, nullptr, &storeRect);

    // 2. Hien thi thong tin
    renderText(renderer, "SHOP", windowWidth / 2 - 30, windowHeight / 2 - 120, 36);
    renderText(renderer, "Coins: " + std::to_string(player->getCoins()), windowWidth / 2 - 80, windowHeight / 2 - 80, 24);

    // Thong tin vat pham
    renderText(renderer, "Resurrection Item (Hoi Sinh)", windowWidth / 2 - 150, windowHeight / 2 - 20, 20);
    renderText(renderer, "Price: " + std::to_string(ITEM_PRICE) + " Coin", windowWidth / 2 - 150, windowHeight / 2 + 5, 18);
    renderText(renderer, "Owned: " + std::to_string(player->getResurrectionCount()), windowWidth / 2 + 50, windowHeight / 2 + 5, 18);


    // 3. Ve cac nut
    SDL_RenderCopy(renderer, isHoverBuy ? buyButtonHover : buyButton, nullptr, &buyRect);
    renderText(renderer, "BUY", buyRect.x + 35, buyRect.y + 10, 20);
    SDL_RenderCopy(renderer, isHoverBack ? backButtonHover : backButton, nullptr, &backRect);
    renderText(renderer, "BACK", backRect.x + 30, backRect.y + 10, 20);
}
