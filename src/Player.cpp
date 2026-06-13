#include "Player.h"
#include "TileMap.h"
#include "Constants.h"
#include <algorithm>
#include <cmath>

Player::Player(float x, float y, const TileMap* map,
               int maxHp, float speed, int damage)
    : GameObject(x, y, ENTITY_SIZE, ENTITY_SIZE)
    , mMap(map)
    , mHp(maxHp), mMaxHp(maxHp)
    , mSpeed(static_cast<float>(speed))
    , mDamage(damage)
    , mFacing(Direction::Down)
    , mVelX(0.f), mVelY(0.f)
    , mAttackTimer(0.f)
    , mAttackCooldown(0.f)
    , mPendingAttackHit(false)
    , mInvulnTimer(0.f)
    , mAttackKeyWasDown(false)
    , mKnockX(0.f), mKnockY(0.f)
    , mKnockTimer(0.f)
    , mHurtTimer(0.f)
    , mDeathTimer(0.f)
    , mShards(0)
    , mHasKey(false)
{
}

void Player::handleInput() {
    mVelX = mVelY = 0.f;

    if (!isAlive())
        return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        mVelY = -1.f; mFacing = Direction::Up;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        mVelY =  1.f; mFacing = Direction::Down;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        mVelX = -1.f; mFacing = Direction::Left;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        mVelX =  1.f; mFacing = Direction::Right;
    }

    if (mVelX != 0.f && mVelY != 0.f) {
        mVelX *= 0.7071f;
        mVelY *= 0.7071f;
    }

    bool attackDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (attackDown && !mAttackKeyWasDown)
        attack();
    mAttackKeyWasDown = attackDown;
}

void Player::attack() {
    if (mAttackCooldown > 0.f)
        return;
    mAttackTimer      = 0.18f;
    mAttackCooldown   = 0.45f;
    mPendingAttackHit = true;
}

void Player::update(float dt) {

    auto tick = [dt](float& t) { if (t > 0.f) { t -= dt; if (t < 0.f) t = 0.f; } };
    tick(mAttackTimer);
    tick(mAttackCooldown);
    tick(mInvulnTimer);
    tick(mKnockTimer);
    tick(mHurtTimer);

    if (!isAlive())
        mDeathTimer += dt;
    else if (mKnockTimer > 0.f)
        moveWithCollision(mKnockX * dt, mKnockY * dt);
    else
        moveWithCollision(mVelX * mSpeed * dt, mVelY * mSpeed * dt);

    int row = (mFacing == Direction::Down)  ? 0
            : (mFacing == Direction::Up)    ? 1
            : (mFacing == Direction::Left)  ? 2 : 3;

    if (!isAlive())              animation.play("death");
    else if (mHurtTimer > 0.f)   animation.play("hurt");
    else if (isAttacking())      animation.play("attack");
    else if (mVelX != 0.f || mVelY != 0.f) animation.play("walk");
    else                         animation.play("idle");

    animation.setRow(row);
    animation.update(dt);
}

void Player::moveWithCollision(float dx, float dy) {
    if (dx != 0.f) {
        sf::FloatRect b(x + dx, y, static_cast<float>(width), static_cast<float>(height));
        if (!mMap->isAreaSolid(b)) x += dx;
    }
    if (dy != 0.f) {
        sf::FloatRect b(x, y + dy, static_cast<float>(width), static_cast<float>(height));
        if (!mMap->isAreaSolid(b)) y += dy;
    }
}

bool Player::takeDamage(int amount) {
    if (mHp <= 0 || mInvulnTimer > 0.f)
        return false;
    mHp -= amount;
    if (mHp < 0) mHp = 0;
    mInvulnTimer = 0.9f;
    mHurtTimer   = 0.3f;
    return true;
}

void Player::knockback(float fromX, float fromY, float strength) {
    float cx = x + width / 2.f;
    float cy = y + height / 2.f;
    float dx = cx - fromX;
    float dy = cy - fromY;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001f) { dx = 0.f; dy = -1.f; len = 1.f; }
    mKnockX = (dx / len) * strength;
    mKnockY = (dy / len) * strength;
    mKnockTimer = 0.15f;
}

void Player::heal(int amount) {
    mHp = std::min(mMaxHp, mHp + amount);
}

void Player::setHp(int hp) {
    mHp = std::max(0, std::min(mMaxHp, hp));
}

int Player::getAttackDamage() const {
    return mDamage;
}

sf::FloatRect Player::getAttackHitbox() const {
    float ax = x, ay = y;
    float w  = static_cast<float>(width);
    float h  = static_cast<float>(height);
    switch (mFacing) {
        case Direction::Up:    ay = y - h; break;
        case Direction::Down:  ay = y + h; break;
        case Direction::Left:  ax = x - w; break;
        case Direction::Right: ax = x + w; break;
    }
    return sf::FloatRect(ax, ay, w, h);
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect(x, y, static_cast<float>(width), static_cast<float>(height));
}

void Player::draw(sf::RenderWindow& window) {

    if (animation.hasSpriteSheet()) {
        animation.draw(window, x + width / 2.f, y + height / 2.f);
    } else {
        sf::RectangleShape body(sf::Vector2f(static_cast<float>(width),
                                             static_cast<float>(height)));
        body.setPosition(x, y);

        bool blink = mInvulnTimer > 0.f &&
                     (static_cast<int>(mInvulnTimer * 12) % 2 == 0);
        body.setFillColor(blink ? sf::Color(120, 255, 120)
                                : sf::Color(70, 200, 90));
        window.draw(body);

        sf::RectangleShape pip(sf::Vector2f(5.f, 5.f));
        pip.setFillColor(sf::Color(20, 60, 25));
        float cx = x + width / 2.f - 2.5f;
        float cy = y + height / 2.f - 2.5f;
        switch (mFacing) {
            case Direction::Up:    cy = y;                break;
            case Direction::Down:  cy = y + height - 5.f; break;
            case Direction::Left:  cx = x;                break;
            case Direction::Right: cx = x + width - 5.f;  break;
        }
        pip.setPosition(cx, cy);
        window.draw(pip);
    }

    if (isAttacking()) {
        sf::FloatRect hb = getAttackHitbox();
        float prog = mAttackTimer / 0.18f;
        float sz = 6.f;
        sf::RectangleShape slash(sf::Vector2f(sz, sz));
        slash.setOrigin(sz / 2.f, sz / 2.f);
        slash.setPosition(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
        slash.setFillColor(sf::Color(255, 255, 255,
                                     static_cast<sf::Uint8>(150 * prog)));
        window.draw(slash);
    }
}
