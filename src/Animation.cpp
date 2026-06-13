#include "Animation.h"

Animation::Animation() {}

void Animation::addClip(const std::string& name, const sf::Texture& tex,
                        int frameWidth, int frameHeight, int frameCount,
                        float frameTime, int rowCount, bool loop) {
    Clip c;
    c.tex       = &tex;
    c.fw        = frameWidth;
    c.fh        = frameHeight;
    c.frames    = frameCount > 0 ? frameCount : 1;
    c.rows      = rowCount   > 0 ? rowCount   : 1;
    c.frameTime = frameTime  > 0.f ? frameTime : 0.1f;
    c.loop      = loop;
    mClips[name] = c;

    if (mCurrentName.empty()) {
        mCurrentName = name;
        mSprite.setTexture(tex, true);
        applyRect();
    }
}

void Animation::setSpriteSheet(const sf::Texture& tex, int frameWidth,
                               int frameHeight, int frameCount, float frameTime,
                               int rowCount) {
    mClips.clear();
    mCurrentName.clear();
    addClip("default", tex, frameWidth, frameHeight, frameCount, frameTime,
            rowCount, true);
}

const Animation::Clip* Animation::current() const {
    auto it = mClips.find(mCurrentName);
    return it == mClips.end() ? nullptr : &it->second;
}

void Animation::play(const std::string& name) {
    if (name == mCurrentName)
        return;
    auto it = mClips.find(name);
    if (it == mClips.end())
        return;
    mCurrentName = name;
    mFrame       = 0;
    mTimer       = 0.f;
    mFinished    = false;
    mSprite.setTexture(*it->second.tex, true);
    applyRect();
}

void Animation::setRow(int row) {
    const Clip* c = current();
    if (!c) return;
    if (row < 0) row = 0;
    if (row >= c->rows) row = c->rows - 1;
    if (row != mRow) {
        mRow = row;
        applyRect();
    }
}

void Animation::applyRect() {
    const Clip* c = current();
    if (!c || !c->tex) return;
    mSprite.setTextureRect(sf::IntRect(mFrame * c->fw, mRow * c->fh,
                                       c->fw, c->fh));
}

void Animation::update(float dt) {
    const Clip* c = current();
    if (!c || c->frames <= 1)
        return;

    mTimer += dt;
    while (mTimer >= c->frameTime) {
        mTimer -= c->frameTime;
        if (mFrame + 1 >= c->frames) {
            if (c->loop) {
                mFrame = 0;
                applyRect();
            } else {
                mFrame    = c->frames - 1;
                mFinished = true;
                applyRect();
                mTimer = 0.f;
                return;
            }
        } else {
            ++mFrame;
            applyRect();
        }
    }
}

void Animation::draw(sf::RenderWindow& window, float centerX, float centerY) {
    const Clip* c = current();
    if (!c || !c->tex)
        return;
    mSprite.setOrigin(c->fw / 2.f, c->fh / 2.f);
    mSprite.setScale(mScale, mScale);
    mSprite.setPosition(centerX, centerY);
    window.draw(mSprite);
}

void Animation::reset() {
    mFrame    = 0;
    mTimer    = 0.f;
    mFinished = false;
    applyRect();
}
