#pragma once
#include "GameObject.h"

class TileMap;

// Player — the hero. Melee fighter controlled with WASD/arrow keys; Space
// swings a short-range hitbox in the facing direction. Its tunables (max HP,
// speed, damage) are passed in by the Game, which reads them from
// assets/config/game.cfg. Demonstrates inheritance (from GameObject) and
// encapsulation: all state is private and exposed through small accessors.
class Player : public GameObject {
public:
    Player(float x, float y, const TileMap* map,
           int maxHp, float speed, int damage);

    void          update(float dt) override;
    void          draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    ObjectType    getType() const override { return ObjectType::Player; }

    // Read the keyboard and set this frame's velocity / trigger attacks.
    void handleInput();
    void attack();

    // Returns true only if damage actually landed (false during the
    // invulnerability window), so the Game knows when to add knockback,
    // screen-shake and the hurt sound.
    bool takeDamage(int amount);

    // Shove the player away from a world point (the attacker's centre).
    void knockback(float fromX, float fromY, float strength);

    // --- Combat queries used by the Game's collision pass ------------------
    bool          hasPendingAttack() const { return mPendingAttackHit; }
    void          clearPendingAttack()     { mPendingAttackHit = false; }
    bool          isAttacking() const       { return mAttackTimer > 0.f; }
    sf::FloatRect getAttackHitbox() const;
    int           getAttackDamage() const;

    // --- Status -------------------------------------------------------------
    int  getHp() const     { return mHp; }
    int  getMaxHp() const  { return mMaxHp; }
    void setHp(int hp);
    bool isAlive() const   { return mHp > 0; }
    void heal(int amount);

    void addShard()        { ++mShards; }
    int  getShards() const { return mShards; }

    void giveKey()         { mHasKey = true; }
    bool hasKey() const    { return mHasKey; }

    // True once the death animation has had time to play out fully.
    bool isDeathDone() const { return mDeathTimer > 0.8f; }

private:
    // Axis-separated movement so the player slides along walls.
    void moveWithCollision(float dx, float dy);

    const TileMap* mMap;   // non-owning: used for wall collision queries

    int       mHp;
    int       mMaxHp;
    float     mSpeed;      // walk speed in px/s (from config)
    int       mDamage;     // melee damage (from config)
    Direction mFacing;

    float mVelX, mVelY;    // this frame's input direction (-1, 0 or +1 per axis)

    float mAttackTimer;     // > 0 while the swing is visible
    float mAttackCooldown;  // > 0 while we cannot swing again
    bool  mPendingAttackHit;// one-shot flag: damage is applied once per swing
    float mInvulnTimer;     // brief i-frames after taking a hit

    bool  mAttackKeyWasDown;// edge detection so holding Space doesn't repeat

    float mKnockX, mKnockY; // active knockback velocity
    float mKnockTimer;      // > 0 while being knocked back

    float mHurtTimer;       // drives the brief "hurt" animation state
    float mDeathTimer;      // counts up after death for the death animation

    int   mShards;          // optional treasure counter (HUD only)
    bool  mHasKey;          // true once the Key pickup was collected
};
