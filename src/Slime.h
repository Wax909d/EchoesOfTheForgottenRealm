#pragma once
#include "Enemy.h"

// Slime — slow, brainless wanderer. Picks a random cardinal direction and
// ambles until it hits a wall or a timer expires, then picks another. Its
// hp/speed/damage arrive in EnemyStats, read from assets/config/game.cfg.
class Slime : public Enemy {
public:
    Slime(float x, float y, const TileMap* map, Player* target,
          const EnemyStats& stats);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    void pickNewDirection();

    float mDirX, mDirY;     // current wander direction (unit axis vector)
    float mChangeTimer;     // seconds until the next direction change
};
