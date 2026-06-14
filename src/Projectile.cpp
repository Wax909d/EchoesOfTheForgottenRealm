#include "Projectile.h"
#include "TileMap.h"
#include <cmath>

Projectile::Projectile(float x, float y, float velX, float velY, int damage,
                       const TileMap* map)
    : GameObject(x, y, 8, 8)
    , mVelX(velX), mVelY(velY)
    , mDamage(damage)
    , mLifetime(3.0f)
    , mMap(map)
{
}

void Projectile::update(float dt) {
    mLifetime -= dt;
    if (mLifetime <= 0.f) {
        active = false;
        return;
    }

    x += mVelX * dt;
    y += mVelY * dt;

    if (mMap && mMap->isSolid(x + width / 2.f, y + height / 2.f))
        active = false;
}

sf::FloatRect Projectile::getBounds() const {
    return sf::FloatRect(x, y, static_cast<float>(width), static_cast<float>(height));
}

void Projectile::draw(sf::RenderWindow& window) {
    float cx = x + width / 2.f;
    float cy = y + height / 2.f;

    if (mTex) {
        sf::Sprite spr(*mTex);
        sf::Vector2u sz = mTex->getSize();
        spr.setOrigin(sz.x / 2.f, sz.y / 2.f);
        spr.setScale(1.6f, 1.6f);
        spr.setPosition(cx, cy);
        spr.setRotation(std::atan2(mVelY, mVelX) * 180.f / 3.14159265f);
        window.draw(spr);
        return;
    }

    sf::RectangleShape shot(sf::Vector2f(static_cast<float>(width),
                                         static_cast<float>(height)));
    shot.setPosition(x, y);
    shot.setFillColor(sf::Color(255, 220, 70));
    window.draw(shot);
}
