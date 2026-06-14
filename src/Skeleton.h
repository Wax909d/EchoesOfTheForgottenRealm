#pragma once
#include "Enemy.h"

// Skeleton — a slow but tanky melee enemy that marches straight toward the
// player while respecting walls. Its hp/speed/damage arrive in EnemyStats,
// read from assets/config/game.cfg.
class Skeleton : public Enemy {
public:
    Skeleton(float x, float y, const TileMap* map, Player* target,
             const EnemyStats& stats);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
};
