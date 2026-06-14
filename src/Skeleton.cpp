#include "Skeleton.h"
#include "Player.h"
#include "Constants.h"

Skeleton::Skeleton(float x, float y, const TileMap* map, Player* target,
                   const EnemyStats& s)
    : Enemy(x, y, ENTITY_SIZE, ENTITY_SIZE, s.hp, s.speed, s.damage, map, target)
{
}

void Skeleton::update(float dt) {
    if (mHitFlash > 0.f) mHitFlash -= dt;
    // While being knocked back, the shove overrides the chase AI entirely.
    if (isKnockedBack()) { updateKnockback(dt); animation.update(dt); return; }

    // Simple pursuit: walk toward the player's position, blocked by walls.
    if (mTarget)
        stepToward(mTarget->getX(), mTarget->getY(), dt);
    animation.update(dt);
}

void Skeleton::draw(sf::RenderWindow& window) {
    // Sprite path; coloured rectangle is the no-texture fallback.
    if (animation.hasSpriteSheet()) { animation.draw(window, x + width / 2.f, y + height / 2.f); return; }

    sf::RectangleShape body(sf::Vector2f(static_cast<float>(width),
                                         static_cast<float>(height)));
    body.setPosition(x, y);
    body.setFillColor(mHitFlash > 0.f ? sf::Color::White
                                      : sf::Color(225, 225, 210));
    window.draw(body);
}
