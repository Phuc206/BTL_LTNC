#pragma once
#include "SDL.h"
#include <string>
#include "../Player.h"
#include "../Support/TextureLoader.h"
#include "../Support/AudioManager.h"

class Player;
class Game;

class Shop {
public:
    Shop(SDL_Renderer* renderer, Player* playerPtr, Game* gamePtr, int windowWidth, int windowHeight);
    ~Shop();

    bool handleInput(SDL_Event& event);
    void draw(SDL_Renderer* renderer);

private:
    void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, int fontSize);

    Player* player;
    Game* game;
    SDL_Renderer* renderer_;
    int windowWidth;
    int windowHeight;

    // Item Constants
    const int ITEM_PRICE = 50;

    // Textures for Shop UI
    SDL_Texture* menuBackground = nullptr;
    SDL_Texture* buyButton = nullptr;
    SDL_Texture* buyButtonHover = nullptr;
    SDL_Texture* backButton = nullptr;
    SDL_Texture* backButtonHover = nullptr;

    // Rects
    SDL_Rect storeRect;
    SDL_Rect buyRect;
    SDL_Rect backRect;
};
