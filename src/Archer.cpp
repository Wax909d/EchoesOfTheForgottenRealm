#include "Archer.h"
#include "Projectile.h"
#include "Player.h"
#include "Constants.h"
#include <cmath>

Archer::Archer(float x, float y, const TileMap* map, Player* target,
               const EnemyStats& s, float fireInterval, float projectileSpeed)
    : Enemy(x, y, ENTITY_SIZE, ENTITY_SIZE, s.hp, s.speed, s.damage, map, target)
    , mFireInterval(fireInterval)
    , mFireTimer(fireInterval)
    , mProjectileSpeed(projectileSpeed)
    , mProjectileDamage(s.damage)
{
}

void Archer::update(float dt) {
    if (mHitFlash > 0.f) mHitFlash -= dt;
    // While being knocked back, the shove suppresses firing for the moment.
    if (isKnockedBack()) { updateKnockback(dt); animation.update(dt); return; }

    // Fire on a fixed cadence; the interval comes from game.cfg.
    mFireTimer -= dt;
    if (mFireTimer <= 0.f) {
        fire();
        mFireTimer = mFireInterval;
    }
    animation.update(dt);
}

// Aim from our centre at the player's centre and spawn one arrow travelling
// along that normalised direction. The arrow is buffered until the Game
// collects it via takeSpawned().
void Archer::fire() {
    if (!mTarget)
        return;

    float cx = x + width  / 2.f;
    float cy = y + height / 2.f;
    float tx = mTarget->getBounds().left + mTarget->getBounds().width  / 2.f;
    float ty = mTarget->getBounds().top  + mTarget->getBounds().height / 2.f;

    float dx = tx - cx;
    float dy = ty - cy;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f)
        return;
    dx /= len;
    dy /= len;

    mPending.push_back(std::make_unique<Projectile>(
        cx - 4.f, cy - 4.f,
        dx * mProjectileSpeed, dy * mProjectileSpeed,
        mProjectileDamage, mMap));
}

std::vector<std::unique_ptr<GameObject>> Archer::takeSpawned() {
    return std::move(mPending);
}

void Archer::draw(sf::RenderWindow& window) {
    // Sprite path; coloured rectangle is the no-texture fallback.
    if (animation.hasSpriteSheet()) { animation.draw(window, x + width / 2.f, y + height / 2.f); return; }

    sf::RectangleShape body(sf::Vector2f(static_cast<float>(width),
                                         static_cast<float>(height)));
    body.setPosition(x, y);
    body.setFillColor(mHitFlash > 0.f ? sf::Color::White
                                      : sf::Color(230, 140, 40));
    window.draw(body);
}
