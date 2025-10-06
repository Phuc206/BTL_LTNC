#include "Orc.h"

Orc::Orc(SDL_Renderer* renderer, Vector2D setPos)
    : Unit(renderer, setPos) {
    textureRun = TextureLoader::loadTexture(renderer, "orc2_run_full.png");
    textureAttack = TextureLoader::loadTexture(renderer, "orc2_attack_full.png");
    textureHurt = TextureLoader::loadTexture(renderer, "orc2_hurt_full.png");
    textureDeath = TextureLoader::loadTexture(renderer, "orc2_death_full.png");

    this->speed = 1.2f;      // Tang toc do tu 0.7 -> 1.5 (Chay nhanh hon)
    this->health = 70;      // Tang mau tu 5 -> 300 (Trau hon)
    this->maxHealth = 70;
    this->attackDamage = 15; // Tang damage tu 5 -> 15 (Manh hon)
    this->attackRange = 0.7f; // Giu khoang cach tan cong
}

