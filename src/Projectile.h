#pragma once
#include "GameObject.h"

class TileMap;

// Projectile — an arrow fired by an Archer. Travels in a fixed direction set
// at creation, damages the player on contact, and expires when its lifetime
// runs out or it hits a wall. Derives from GameObject so it lives in the same
// polymorphic entity list as everything else.
class Projectile : public GameObject {
public:
    Projectile(float x, float y, float velX, float velY, int damage,
               const TileMap* map);

    void          update(float dt) override;
    void          draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    void setTexture(const sf::Texture* tex) { mTex = tex; }
    ObjectType    getType() const override { return ObjectType::Projectile; }

    int getDamage() const { return mDamage; }

private:
    float          mVelX, mVelY;
    int            mDamage;
    float          mLifetime;
    const TileMap* mMap;
    const sf::Texture* mTex = nullptr;
};
