#include "Vampire.h"

Vampire::Vampire(SDL_Renderer* renderer, Vector2D setPos)
    : Unit(renderer, setPos) {
    textureRun = TextureLoader::loadTexture(renderer, "Vampires2_Run_full.png");
    textureAttack = TextureLoader::loadTexture(renderer, "Vampires2_Attack_full.png");
    textureHurt = TextureLoader::loadTexture(renderer, "Vampires2_Hurt_full.png");
    textureDeath = TextureLoader::loadTexture(renderer, "Vampires2_Death_full.png");

    this->speed = 1.5f;      // Tang toc do tu 0.7 -> 1.5 (Chay nhanh hon)
    this->health = 50;      // Tang mau tu 5 -> 300 (Trau hon)
    this->maxHealth = 50;
    this->attackDamage = 25; // Tang damage tu 5 -> 15 (Manh hon)
    this->attackRange = 0.8f; // Giu khoang cach tan cong

}
