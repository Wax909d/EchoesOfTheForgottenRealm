#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

// Animation — a small frame-based sprite animator, composed by every
// GameObject. It holds named clips (e.g. "idle", "walk", "attack"); each clip
// is a horizontal strip of equal frames in a spritesheet, optionally arranged
// in rows for 4-directional facing. update(dt) advances the active clip using
// delta time; play() switches clips; draw() blits the current frame centred on
// the entity. The game runs without art too — entities fall back to coloured
// shapes when no spritesheet is set.
class Animation {
public:
    Animation();

    void setSpriteSheet(const sf::Texture& tex, int frameWidth, int frameHeight,
                        int frameCount, float frameTime, int rowCount = 1);

    void addClip(const std::string& name, const sf::Texture& tex,
                 int frameWidth, int frameHeight, int frameCount,
                 float frameTime, int rowCount, bool loop);
    void play(const std::string& name);
    bool isFinished() const { return mFinished; }

    void setRow(int row);
    void setScale(float scale) { mScale = scale; }

    void update(float dt);
    void draw(sf::RenderWindow& window, float centerX, float centerY);

    bool hasSpriteSheet() const { return !mClips.empty(); }
    void reset();

private:
    struct Clip {
        const sf::Texture* tex = nullptr;
        int   fw = 0, fh = 0, frames = 1, rows = 1;
        float frameTime = 0.1f;
        bool  loop = true;
    };
    const Clip* current() const;
    void applyRect();

    std::map<std::string, Clip> mClips;
    std::string mCurrentName;
    sf::Sprite  mSprite;
    int   mFrame = 0;
    int   mRow = 0;
    float mTimer = 0.f;
    bool  mFinished = false;
    float mScale = 1.f;
};
