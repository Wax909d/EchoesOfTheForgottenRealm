#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>

// Archer — a stationary ranged enemy that fires a Projectile (arrow) at the
// player on a fixed interval. Stats and fire parameters come from
// assets/config/game.cfg. Newly fired arrows are buffered in mPending and
// handed to the Game each frame through takeSpawned(), so the Archer never
// needs direct access to the world entity list.
class Archer : public Enemy {
public:
    Archer(float x, float y, const TileMap* map, Player* target,
           const EnemyStats& stats, float fireInterval, float projectileSpeed);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    // Hand any freshly fired projectiles to the Game (which owns the world).
    std::vector<std::unique_ptr<GameObject>> takeSpawned() override;

private:
    void fire();

    float mFireInterval;       // seconds between shots
    float mFireTimer;          // counts down to the next shot
    float mProjectileSpeed;    // arrow speed in px/s
    int   mProjectileDamage;   // arrow damage on hit

    std::vector<std::unique_ptr<GameObject>> mPending;
};
