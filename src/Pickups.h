#pragma once
#include "GameObject.h"

class Player;

// Pickup — ABSTRACT base for every collectable. The key design point is the
// pure virtual apply(Player*): the Game collides the player with a Pickup*
// and calls apply() without knowing which concrete pickup it is; each
// subclass implements a different effect (polymorphism). The base also owns
// the shared bob animation and a coloured-gem fallback renderer used when a
// subclass has no texture.
class Pickup : public GameObject {
public:
    Pickup(float x, float y);

    // The polymorphic effect: heal, score, or unlock - decided by the subclass.
    virtual void apply(Player* player) = 0;

    void          update(float dt) override;
    void          draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    ObjectType    getType() const override { return ObjectType::Pickup; }

protected:
    // Fallback gem colour when the pickup has no texture.
    virtual sf::Color colour() const = 0;

    float mBob;   // phase of the gentle idle bobbing motion
};

// HealthPotion — restores a few hit points when collected.
class HealthPotion : public Pickup {
public:
    HealthPotion(float x, float y) : Pickup(x, y) {}
    void apply(Player* player) override;
protected:
    sf::Color colour() const override { return sf::Color(240, 90, 130); }
};

// RuneShard — optional treasure; increments the player's shard count shown
// in the HUD. Purely score, no gameplay gate.
class RuneShard : public Pickup {
public:
    RuneShard(float x, float y) : Pickup(x, y) {}
    void apply(Player* player) override;
protected:
    sf::Color colour() const override { return sf::Color(245, 215, 70); }
};

// Key — unlocks the level exit when collected. Renders as a cyan gem via the
// base-class fallback since there is no key texture.
class Key : public Pickup {
public:
    Key(float x, float y) : Pickup(x, y) {}
    void apply(Player* player) override;
protected:
    sf::Color colour() const override { return sf::Color(120, 220, 255); }
};
