#pragma once
#include "GameObject.h"

class TileMap;
class Player;

// Plain data passed into every enemy so its tunables come from game.cfg
// instead of being hard-coded in each subclass.
struct EnemyStats {
    int   hp;
    float speed;   // pixels per second
    int   damage;  // contact damage in half-hearts
};

// Enemy — ABSTRACT middle layer of the hierarchy. Holds everything the three
// concrete enemies (Slime, Skeleton, Archer) share: hit points, speed,
// contact damage, a pointer to the map for collision queries and to the
// player for AI targeting, plus shared movement/knockback helpers. It
// deliberately does NOT implement update()/draw(), so it stays abstract and
// each subclass supplies its own behaviour (polymorphism).
class Enemy : public GameObject {
public:
    Enemy(float x, float y, int width, int height,
          int hp, float speed, int damage,
          const TileMap* map, Player* target);

    ObjectType    getType() const override { return ObjectType::Enemy; }
    sf::FloatRect getBounds() const override;

    void takeDamage(int amount);
    bool isAlive() const { return mHp > 0; }
    int  getDamage() const { return mDamage; }

    // Shove this enemy away from a world point (e.g. the player's sword).
    void knockback(float fromX, float fromY, float strength);

protected:
    // Axis-separated movement that respects walls: each axis is tried
    // independently so the enemy can slide along a wall it brushes.
    void moveWithCollision(float dx, float dy);

    // Move toward a world point at mSpeed, still respecting walls.
    void stepToward(float targetX, float targetY, float dt);

    // Knockback helpers shared by every enemy's update(): while the timer is
    // running the shove overrides normal AI movement.
    bool isKnockedBack() const { return mKnockTimer > 0.f; }
    void updateKnockback(float dt);

    int   mHp;
    int   mMaxHp;
    float mSpeed;
    int   mDamage;

    const TileMap* mMap;     // non-owning: the level owns the map
    Player*        mTarget;  // non-owning observer used for AI decisions

    float mHitFlash;         // > 0 briefly after being struck (white flash)

    float mKnockX, mKnockY;  // active knockback velocity
    float mKnockTimer;       // > 0 while being knocked back
};
