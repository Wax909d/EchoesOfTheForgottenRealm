#include "Pickups.h"
#include "Player.h"
#include "Constants.h"
#include <cmath>

Pickup::Pickup(float x, float y)
    : GameObject(x, y, ENTITY_SIZE, ENTITY_SIZE)
    , mBob(0.f)
{
}

void Pickup::update(float dt) {
    mBob += dt * 4.f;          // advance the bob phase
    animation.update(dt);
}

sf::FloatRect Pickup::getBounds() const {
    return sf::FloatRect(x, y, static_cast<float>(width), static_cast<float>(height));
}

void Pickup::draw(sf::RenderWindow& window) {
    // sin(mBob) makes the item float up and down a couple of pixels so it
    // reads as "collectable" even when it has no animation frames.
    float offset = std::sin(mBob) * 2.f;

    if (animation.hasSpriteSheet()) {
        animation.draw(window, x + width / 2.f, y + height / 2.f + offset);
        return;
    }

    // No texture: draw the subclass-coloured gem fallback.
    sf::RectangleShape gem(sf::Vector2f(static_cast<float>(width) - 4.f,
                                        static_cast<float>(height) - 4.f));
    gem.setPosition(x + 2.f, y + 2.f + offset);
    gem.setFillColor(colour());
    gem.setOutlineThickness(1.f);
    gem.setOutlineColor(sf::Color::White);
    window.draw(gem);
}

void HealthPotion::apply(Player* player) {
    player->heal(3);
}

void RuneShard::apply(Player* player) {
    player->addShard();
}

void Key::apply(Player* player) {
    player->giveKey();
}
