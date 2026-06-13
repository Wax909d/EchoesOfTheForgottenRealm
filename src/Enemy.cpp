#include "Enemy.h"
#include "TileMap.h"
#include "Player.h"
#include <cmath>

Enemy::Enemy(float x, float y, int width, int height,
             int hp, float speed, int damage,
             const TileMap* map, Player* target)
    : GameObject(x, y, width, height)
    , mHp(hp), mMaxHp(hp)
    , mSpeed(speed)
    , mDamage(damage)
    , mMap(map)
    , mTarget(target)
    , mHitFlash(0.f)
    , mKnockX(0.f), mKnockY(0.f)
    , mKnockTimer(0.f)
{
}

sf::FloatRect Enemy::getBounds() const {
    return sf::FloatRect(x, y, static_cast<float>(width), static_cast<float>(height));
}

void Enemy::takeDamage(int amount) {
    mHp -= amount;
    if (mHp < 0) mHp = 0;
    mHitFlash = 0.12f;
    if (mHp <= 0)
        active = false;
}

void Enemy::moveWithCollision(float dx, float dy) {
    if (dx != 0.f) {
        sf::FloatRect b(x + dx, y, static_cast<float>(width), static_cast<float>(height));
        if (!mMap->isAreaSolid(b)) x += dx;
    }
    if (dy != 0.f) {
        sf::FloatRect b(x, y + dy, static_cast<float>(width), static_cast<float>(height));
        if (!mMap->isAreaSolid(b)) y += dy;
    }
}

void Enemy::stepToward(float targetX, float targetY, float dt) {
    float dx = targetX - x;
    float dy = targetY - y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f)
        return;
    dx /= len;
    dy /= len;
    moveWithCollision(dx * mSpeed * dt, dy * mSpeed * dt);
}

void Enemy::knockback(float fromX, float fromY, float strength) {
    float cx = x + width / 2.f;
    float cy = y + height / 2.f;
    float dx = cx - fromX;
    float dy = cy - fromY;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f) { dx = 0.f; dy = -1.f; len = 1.f; }
    mKnockX = (dx / len) * strength;
    mKnockY = (dy / len) * strength;
    mKnockTimer = 0.16f;
}

void Enemy::updateKnockback(float dt) {
    moveWithCollision(mKnockX * dt, mKnockY * dt);
    mKnockTimer -= dt;
}
