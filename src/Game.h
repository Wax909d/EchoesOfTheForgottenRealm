#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <string>
#include "GameObject.h"
#include "TileMap.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "Config.h"

enum class GameState {
    Menu,
    Playing,
    Paused,
    GameOver,
    Win
};

class Player;

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    void loadLevel(int levelNumber, int startHp);
    void spawnEnemiesRandomly();   // requirement 9: random enemy placement
    void handleCollisions();
    void absorbSpawned();
    void removeInactive();

    void applyTexture(GameObject* obj, const std::string& key,
                      int frameW, int frameH, int frameCount, float frameTime,
                      int rowCount = 1);

    void updateCamera();
    void toggleFullscreen();
    void applyLetterbox();
    void triggerShake(float magnitude, float duration);
    void spawnBurst(float x, float y, sf::Color colour, int count);
    void updateParticles(float dt);
    void drawParticles();
    void drawHearts();
    std::string formatTime(float seconds) const;

    void drawTextLine(const std::string& str, unsigned int size,
                      sf::Color colour, float centreX, float topY);
    void drawDimPanel(sf::Uint8 alpha);
    void drawVolumeBar(const std::string& label, int value, float y,
                       bool selected);
    void adjustVolume(int delta);

    sf::RenderWindow mWindow;
    sf::View         mWorldView;
    sf::View         mUiView;
    GameState        mState;
    int              mCurrentLevel;

    float mShakeTimer = 0.f;
    float mShakeDur   = 0.f;
    float mShakeMag   = 0.f;

    struct Particle {
        float x, y, vx, vy, life, maxLife;
        sf::Color colour;
    };
    std::vector<Particle> mParticles;

    std::string mMessage;
    float       mMessageTimer = 0.f;

    float mPlayTime      = 0.f;
    int   mEnemiesKilled = 0;

    int  mMusicVol = 40;   // 0..100
    int  mSfxVol   = 80;
    int  mMenuSel  = 0;    // 0 = music slider, 1 = sfx slider
    bool mFullscreen   = false;

    sf::Font mFont;
    sf::Text mHud;

    TileMap mMap;
    std::vector<std::unique_ptr<GameObject>> mObjects;
    Player* mPlayer;

    TextureManager mTextures;
    AudioManager   mAudio;
    sf::Music      mMusic;
    Config         mConfig;   // tunable gameplay parameters from game.cfg

    static constexpr int   LAST_LEVEL = 3;
};
