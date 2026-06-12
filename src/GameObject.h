#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Animation.h"

// Four cardinal facings, used for movement and choosing the player's sprite row.
enum class Direction { Up, Down, Left, Right };

// A cheap category tag. Collision dispatch uses dynamic_cast (run-time type
// info) rather than this enum, so it is kept only as a coarse hint.
enum class ObjectType { Player, Enemy, Projectile, Pickup };

// GameObject — ABSTRACT base for everything that lives in the world (player,
// enemies, projectiles, pickups). It owns position/size, an active flag and an
// Animation, and declares the polymorphic interface every entity must provide:
//
//   update(dt)   advance state using delta time
//   draw(window) render this frame
//   getBounds()  axis-aligned box for collision
//   getType()    coarse category tag
//
// Storing all entities as unique_ptr<GameObject> in one vector and calling
// these virtuals is what makes the game loop fully polymorphic. Demonstrates
// abstraction, encapsulation (data is protected) and composition (every
// object owns an Animation).
class GameObject {
public:
    GameObject(float x, float y, int width, int height)
        : x(x), y(y), width(width), height(height), active(true) {}

    virtual ~GameObject() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual ObjectType getType() const = 0;

    virtual std::vector<std::unique_ptr<GameObject>> takeSpawned() {
        return {};
    }

    void setSprite(const sf::Texture& tex, int frameW, int frameH,
                   int frameCount, float frameTime, int rowCount = 1) {
        animation.setSpriteSheet(tex, frameW, frameH, frameCount, frameTime,
                                 rowCount);
    }

    void setSpriteScale(float scale) { animation.setScale(scale); }

    void addAnimationClip(const std::string& name, const sf::Texture& tex,
                          int frameW, int frameH, int frameCount,
                          float frameTime, int rowCount, bool loop) {
        animation.addClip(name, tex, frameW, frameH, frameCount, frameTime,
                          rowCount, loop);
    }

    bool  isActive() const { return active; }
    void  setActive(bool a) { active = a; }
    float getX() const { return x; }
    float getY() const { return y; }

protected:
    float     x, y;
    int       width, height;
    bool      active;
    Animation animation;
};
