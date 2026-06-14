#include "Slime.h"
#include "Constants.h"
#include <cstdlib>

Slime::Slime(float x, float y, const TileMap* map, Player* target,
             const EnemyStats& s)
    : Enemy(x, y, ENTITY_SIZE, ENTITY_SIZE, s.hp, s.speed, s.damage, map, target)
    , mDirX(0.f), mDirY(0.f)
    , mChangeTimer(0.f)
{
    pickNewDirection();
}

// Choose a random cardinal direction and a random 1.5-2.5s walk duration so
// slimes do not all turn in lockstep.
void Slime::pickNewDirection() {
    static const float dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
    int i = std::rand() % 4;
    mDirX = dirs[i][0];
    mDirY = dirs[i][1];
    mChangeTimer = 1.5f + static_cast<float>(std::rand() % 100) / 100.f;
}

void Slime::update(float dt) {
    if (mHitFlash > 0.f) mHitFlash -= dt;
    // While being knocked back, the shove overrides the wander AI entirely.
    if (isKnockedBack()) { updateKnockback(dt); animation.update(dt); return; }

    mChangeTimer -= dt;
    if (mChangeTimer <= 0.f)
        pickNewDirection();

    float oldX = x, oldY = y;
    moveWithCollision(mDirX * mSpeed * dt, mDirY * mSpeed * dt);

    // Bumped into a wall (no movement happened)? Turn somewhere else.
    if (x == oldX && y == oldY)
        pickNewDirection();

    animation.update(dt);
}

void Slime::draw(sf::RenderWindow& window) {
    // Sprite path; the coloured rectangle below is only a fallback when no
    // texture was loaded, so the game still runs without any art assets.
    if (animation.hasSpriteSheet()) { animation.draw(window, x + width / 2.f, y + height / 2.f); return; }

    sf::RectangleShape body(sf::Vector2f(static_cast<float>(width),
                                         static_cast<float>(height)));
    body.setPosition(x, y);
    body.setFillColor(mHitFlash > 0.f ? sf::Color::White
                                      : sf::Color(120, 200, 80));
    window.draw(body);
}
