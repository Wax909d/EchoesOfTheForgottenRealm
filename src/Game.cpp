#include "Game.h"
#include "Constants.h"
#include "Player.h"
#include "Enemy.h"
#include "Slime.h"
#include "Archer.h"
#include "Skeleton.h"
#include "Projectile.h"
#include "Pickups.h"

#include <SFML/System/Clock.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>

Game::Game()
    : mWindow(sf::VideoMode(WINDOW_WIDTH * WINDOW_SCALE, WINDOW_HEIGHT * WINDOW_SCALE),
              "Echoes of the Forgotten Realm",
              sf::Style::Titlebar | sf::Style::Close)
    , mState(GameState::Menu)
    , mCurrentLevel(1)
    , mPlayer(nullptr)
{
    mWindow.setFramerateLimit(60);
    // High-resolution seed so random enemy layouts differ on every launch,
    // even two runs started within the same second.
    std::srand(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    mWorldView.setSize(WINDOW_WIDTH / CAMERA_ZOOM, WINDOW_HEIGHT / CAMERA_ZOOM);
    mWorldView.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);

    mUiView.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    mUiView.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    applyLetterbox();

    if (!mFont.loadFromFile("assets/fonts/PressStart2P.ttf"))
        mFont.loadFromFile("/System/Library/Fonts/Helvetica.ttc");

    mHud.setFont(mFont);
    mHud.setCharacterSize(8);
    mHud.setFillColor(sf::Color::White);
    mHud.setPosition(72.f, 6.f);

    mTextures.load("menu_bg",        "assets/textures/menu_bg.png");
    mTextures.load("player_idle",    "assets/textures/player_idle.png");
    mTextures.load("player_walk",    "assets/textures/player_walk.png");
    mTextures.load("player_attack",  "assets/textures/player_attack.png");
    mTextures.load("player_hurt",    "assets/textures/player_hurt.png");
    mTextures.load("player_death",   "assets/textures/player_death.png");
    mTextures.load("slime",     "assets/textures/slime.png");
    mTextures.load("archer",    "assets/textures/archer.png");
    mTextures.load("skeleton",  "assets/textures/skeleton.png");
    mTextures.load("heart",     "assets/textures/heart.png");
    mTextures.load("potion",    "assets/textures/potion.png");
    mTextures.load("shard",     "assets/textures/shard.png");
    mTextures.load("wall",      "assets/textures/wall.png");
    mTextures.load("floor",     "assets/textures/floor.png");
    mTextures.load("exit",      "assets/textures/exit.png");
    mTextures.load("exit_open", "assets/textures/exit_open.png");
    mTextures.load("arrow",     "assets/textures/arrow.png");

    mAudio.loadSound("hit",        "assets/audio/hit.wav");
    mAudio.loadSound("swing",      "assets/audio/swing.wav");
    mAudio.loadSound("pickup",     "assets/audio/pickup.wav");
    mAudio.loadSound("hurt",       "assets/audio/hurt.wav");

    // Load tunable gameplay parameters. If the file is missing, every getter
    // quietly falls back to a sensible default so the game still runs.
    mConfig.loadFromFile("assets/config/game.cfg");

    mAudio.setMasterVolume(static_cast<float>(mSfxVol));
    if (mMusic.openFromFile("assets/audio/music.wav")) {
        mMusic.setLoop(true);
        mMusic.setVolume(static_cast<float>(mMusicVol));
        mMusic.play();
    }
}

void Game::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        if (event.type != sf::Event::KeyPressed)
            continue;

        if (event.key.code == sf::Keyboard::F11) {
            toggleFullscreen();
            continue;
        }

        switch (mState) {
            case GameState::Menu:
                if (event.key.code == sf::Keyboard::Return) {
                    mPlayTime      = 0.f;
                    mEnemiesKilled = 0;
                    loadLevel(1, mConfig.getInt("player.maxHp", 8));
                    mState = GameState::Playing;
                }
                if (event.key.code == sf::Keyboard::Escape)
                    mWindow.close();
                if (event.key.code == sf::Keyboard::Up)
                    mMenuSel = 0;
                if (event.key.code == sf::Keyboard::Down)
                    mMenuSel = 1;
                if (event.key.code == sf::Keyboard::Left)
                    adjustVolume(-10);
                if (event.key.code == sf::Keyboard::Right)
                    adjustVolume(+10);
                break;

            case GameState::Playing:
                if (event.key.code == sf::Keyboard::Escape)
                    mState = GameState::Paused;
                break;

            case GameState::Paused:
                if (event.key.code == sf::Keyboard::Escape)
                    mState = GameState::Playing;
                break;

            case GameState::GameOver:
            case GameState::Win:
                if (event.key.code == sf::Keyboard::Return)
                    mState = GameState::Menu;
                break;
        }
    }
}

void Game::loadLevel(int levelNumber, int startHp) {
    mCurrentLevel = levelNumber;
    mObjects.clear();
    mPlayer = nullptr;
    mParticles.clear();
    mMessageTimer = 0.f;

    std::string path = "assets/levels/level" + std::to_string(levelNumber) + ".txt";
    mMap.loadFromFile(path);
    mMap.setTextures(mTextures.get("wall"), mTextures.get("floor"),
                     mTextures.get("exit"), mTextures.get("exit_open"));

    // Create the player with stats read from game.cfg (with safe fallbacks).
    sf::Vector2f ps = mMap.getPlayerSpawn();
    auto player = std::make_unique<Player>(
        ps.x + ENTITY_INSET, ps.y + ENTITY_INSET, &mMap,
        mConfig.getInt  ("player.maxHp", 8),
        mConfig.getFloat("player.speed", 95.f),
        mConfig.getInt  ("player.damage", 2));
    player->setHp(startHp);                 // carry HP forward between levels
    mPlayer = player.get();
    mObjects.push_back(std::move(player));

    auto playerClip = [&](const char* name, const char* key, int frames,
                          float ft, bool loop) {
        const sf::Texture* t = mTextures.get(key);
        if (t) mPlayer->addAnimationClip(name, *t, 32, 32, frames, ft, 4, loop);
    };
    playerClip("idle",   "player_idle",   4, 0.18f, true);
    playerClip("walk",   "player_walk",   6, 0.08f, true);
    playerClip("attack", "player_attack", 4, 0.05f, false);
    playerClip("hurt",   "player_hurt",   2, 0.10f, false);
    playerClip("death",  "player_death",  8, 0.09f, false);
    mPlayer->setSpriteScale(PLAYER_SPRITE_SCALE);

    // Maps only carry pickups now (potion, shard, key); enemies are random.
    for (const auto& s : mMap.getSpawns()) {
        float ex = s.x + ENTITY_INSET;
        float ey = s.y + ENTITY_INSET;
        switch (s.type) {
            case 'H':
                mObjects.push_back(std::make_unique<HealthPotion>(ex, ey));
                applyTexture(mObjects.back().get(), "potion", 16, 16, 1, 0.5f);
                break;
            case 'R':
                mObjects.push_back(std::make_unique<RuneShard>(ex, ey));
                applyTexture(mObjects.back().get(), "shard", 16, 16, 1, 0.5f);
                break;
            case 'Y':   // Key - drawn as a cyan gem via the base-class fallback
                mObjects.push_back(std::make_unique<Key>(ex, ey));
                break;
            default:
                break;
        }
    }

    spawnEnemiesRandomly();   // requirement 9: random enemy placement
}

// Drops config-defined numbers of each enemy type onto RANDOM free floor
// tiles. Tiles near the player spawn are excluded so the player never starts
// on top of a monster, and each tile is used at most once.
void Game::spawnEnemiesRandomly() {
    std::vector<sf::Vector2f> tiles = mMap.getFreeFloorTiles();

    // Carve out a safety ring around the player spawn.
    const sf::Vector2f ps = mMap.getPlayerSpawn();
    const float safe = TILE_SIZE * 4.f;
    tiles.erase(std::remove_if(tiles.begin(), tiles.end(),
        [&](const sf::Vector2f& t) {
            float dx = t.x - ps.x, dy = t.y - ps.y;
            return dx * dx + dy * dy < safe * safe;
        }), tiles.end());

    const std::string lvl = "level" + std::to_string(mCurrentLevel);

    // Generic placer: pulls `count` random tiles and hands each to `makeOne`.
    auto place = [&](int count, auto makeOne) {
        for (int i = 0; i < count && !tiles.empty(); ++i) {
            int idx = std::rand() % static_cast<int>(tiles.size());
            sf::Vector2f t = tiles[idx];
            tiles.erase(tiles.begin() + idx);   // each tile used at most once
            makeOne(t.x + ENTITY_INSET, t.y + ENTITY_INSET);
        }
    };

    // Read each enemy's tunables once from the config.
    EnemyStats slime{ mConfig.getInt("slime.hp", 3),
                      mConfig.getFloat("slime.speed", 32.f),
                      mConfig.getInt("slime.damage", 1) };
    EnemyStats skel { mConfig.getInt("skeleton.hp", 4),
                      mConfig.getFloat("skeleton.speed", 38.f),
                      mConfig.getInt("skeleton.damage", 1) };
    EnemyStats arch { mConfig.getInt("archer.hp", 3),
                      mConfig.getFloat("archer.speed", 0.f),
                      mConfig.getInt("archer.damage", 1) };
    const float fireRate = mConfig.getFloat("archer.fireRate", 2.4f);
    const float arrowSpd = mConfig.getFloat("archer.arrowSpeed", 120.f);

    place(mConfig.getInt(lvl + ".slimes", 0), [&](float x, float y) {
        mObjects.push_back(std::make_unique<Slime>(x, y, &mMap, mPlayer, slime));
        applyTexture(mObjects.back().get(), "slime", 32, 32, 6, 0.16f);
        mObjects.back()->setSpriteScale(1.05f);
    });
    place(mConfig.getInt(lvl + ".skeletons", 0), [&](float x, float y) {
        mObjects.push_back(std::make_unique<Skeleton>(x, y, &mMap, mPlayer, skel));
        applyTexture(mObjects.back().get(), "skeleton", 16, 16, 2, 0.22f);
    });
    place(mConfig.getInt(lvl + ".archers", 0), [&](float x, float y) {
        mObjects.push_back(
            std::make_unique<Archer>(x, y, &mMap, mPlayer, arch, fireRate, arrowSpd));
        applyTexture(mObjects.back().get(), "archer", 32, 32, 4, 0.22f);
        mObjects.back()->setSpriteScale(1.05f);
    });
}

void Game::applyTexture(GameObject* obj, const std::string& key,
                        int frameW, int frameH, int frameCount, float frameTime,
                        int rowCount) {
    const sf::Texture* tex = mTextures.get(key);
    if (tex)
        obj->setSprite(*tex, frameW, frameH, frameCount, frameTime, rowCount);
}

void Game::update(float dt) {
    if (mState != GameState::Playing)
        return;

    mPlayTime += dt;
    if (mShakeTimer   > 0.f) mShakeTimer   -= dt;
    if (mMessageTimer > 0.f) mMessageTimer -= dt;
    updateParticles(dt);

    mPlayer->handleInput();
    for (auto& obj : mObjects)
        obj->update(dt);

    absorbSpawned();

    handleCollisions();

    removeInactive();

    if (!mPlayer->isAlive()) {
        // Let the death animation play out, then show the Game Over screen.
        if (mPlayer->isDeathDone())
            mState = GameState::GameOver;
        return;
    }
    if (mMap.isExitOpen() &&
        mPlayer->getBounds().intersects(mMap.getExitBounds())) {
        if (mCurrentLevel >= LAST_LEVEL) {
            mState = GameState::Win;
        } else {
            loadLevel(mCurrentLevel + 1, mPlayer->getHp());
        }
        return;
    }

    mHud.setString(
        "L" + std::to_string(mCurrentLevel) +
        "   SHARDS " + std::to_string(mPlayer->getShards()) +
        (mPlayer->hasKey() ? "   KEY" : ""));
}

void Game::absorbSpawned() {
    std::vector<std::unique_ptr<GameObject>> spawned;
    for (auto& obj : mObjects) {
        auto more = obj->takeSpawned();
        for (auto& m : more)
            spawned.push_back(std::move(m));
    }
    for (auto& s : spawned) {
        // dynamic_cast: only succeeds (non-null) when the object really is a
        // Projectile, so we can give arrows their texture without a type tag.
        if (auto* proj = dynamic_cast<Projectile*>(s.get()))
            proj->setTexture(mTextures.get("arrow"));
        mObjects.push_back(std::move(s));
    }
}

void Game::handleCollisions() {
    bool          melee = mPlayer->hasPendingAttack();
    bool          hitSomething = false;
    sf::FloatRect hb    = melee ? mPlayer->getAttackHitbox() : sf::FloatRect();
    sf::FloatRect pb    = mPlayer->getBounds();
    float         pcx   = pb.left + pb.width  / 2.f;
    float         pcy   = pb.top  + pb.height / 2.f;

    for (auto& obj : mObjects) {
        GameObject* o = obj.get();
        if (!o->isActive() || o == mPlayer)
            continue;

        // dynamic_cast does a run-time-checked downcast: it yields a valid
        // pointer only when `o` really IS that subtype, and nullptr otherwise.
        // This is the polymorphic alternative to switching on a type tag.
        if (Enemy* e = dynamic_cast<Enemy*>(o)) {
            sf::FloatRect eb = e->getBounds();
            float ecx = eb.left + eb.width  / 2.f;
            float ecy = eb.top  + eb.height / 2.f;

            // Player's sword connecting with this enemy.
            if (melee && hb.intersects(eb)) {
                e->takeDamage(mPlayer->getAttackDamage());
                hitSomething = true;
                if (!e->isActive()) {
                    spawnBurst(ecx, ecy, sf::Color(255, 240, 200), 16);
                    ++mEnemiesKilled;
                } else {
                    e->knockback(pcx, pcy, 130.f);
                }
            }
            // Surviving enemy touching the player deals contact damage.
            if (e->isActive() && eb.intersects(pb) &&
                mPlayer->takeDamage(e->getDamage())) {
                mPlayer->knockback(ecx, ecy, 150.f);
                triggerShake(4.f, 0.25f);
                mAudio.play("hurt");
            }
        }
        else if (Projectile* p = dynamic_cast<Projectile*>(o)) {
            sf::FloatRect prb = p->getBounds();
            if (prb.intersects(pb)) {
                if (mPlayer->takeDamage(p->getDamage())) {
                    mPlayer->knockback(prb.left + prb.width / 2.f,
                                       prb.top  + prb.height / 2.f, 130.f);
                    triggerShake(3.f, 0.2f);
                    mAudio.play("hurt");
                }
                p->setActive(false);   // arrows vanish on impact
            }
        }
        else if (Pickup* pk = dynamic_cast<Pickup*>(o)) {
            sf::FloatRect kb = pk->getBounds();
            if (kb.intersects(pb)) {
                pk->apply(mPlayer);    // virtual: heal / score / unlock
                pk->setActive(false);
                mAudio.play("pickup");
                spawnBurst(kb.left + kb.width / 2.f, kb.top + kb.height / 2.f,
                           sf::Color(245, 215, 70), 16);
            }
        }
    }

    if (melee) {
        mAudio.play("swing");
        if (hitSomething)
            mAudio.play("hit");
        mPlayer->clearPendingAttack();
    }

    // Key-based exit unlock (replaces the old shard-count gate).
    if (mPlayer->hasKey() && !mMap.isExitOpen()) {
        mMap.openExit();
        mMessage      = "KEY FOUND - EXIT OPEN!";
        mMessageTimer = 2.5f;
    }
}

void Game::removeInactive() {
    mObjects.erase(
        std::remove_if(mObjects.begin(), mObjects.end(),
            [](const std::unique_ptr<GameObject>& o) { return !o->isActive(); }),
        mObjects.end());
}

void Game::render() {
    mWindow.clear(sf::Color(18, 18, 26));
    mWindow.setView(mUiView);
    const float cx = WINDOW_WIDTH / 2.f;
    const sf::Color gold(245, 215, 70);

    switch (mState) {
        case GameState::Menu: {
            const sf::Texture* bg = mTextures.get("menu_bg");
            if (bg) {
                sf::Sprite s(*bg);
                mWindow.draw(s);
            }
            drawTextLine("ECHOES OF THE",   18, gold,             cx, 62.f);
            drawTextLine("FORGOTTEN REALM", 18, gold,             cx, 90.f);

            drawVolumeBar("MUSIC", mMusicVol, 150.f, mMenuSel == 0);
            drawVolumeBar("SFX",   mSfxVol,   172.f, mMenuSel == 1);

            drawTextLine("ENTER - START",   10, sf::Color::White, cx, 214.f);
            drawTextLine("ESC - QUIT",      10, sf::Color::White, cx, 234.f);
            drawTextLine("UP/DOWN PICK   LEFT/RIGHT VOLUME", 8,
                         sf::Color(190, 190, 205), cx, 298.f);
            drawTextLine("WASD MOVE   SPACE ATTACK   F11 FULLSCREEN", 8,
                         sf::Color(190, 190, 205), cx, 320.f);
            break;
        }

        case GameState::Playing:
        case GameState::Paused:

            updateCamera();
            mWindow.setView(mWorldView);
            mMap.draw(mWindow);
            for (auto& obj : mObjects)
                obj->draw(mWindow);
            drawParticles();

            mWindow.setView(mUiView);
            {
                sf::RectangleShape bar(sf::Vector2f(WINDOW_WIDTH, 20.f));
                bar.setFillColor(sf::Color(0, 0, 0, 130));
                mWindow.draw(bar);
            }
            drawHearts();
            mWindow.draw(mHud);
            if (mMessageTimer > 0.f) {
                sf::Uint8 a = static_cast<sf::Uint8>(
                    255 * std::min(1.f, mMessageTimer / 0.6f));
                drawTextLine(mMessage, 9, sf::Color(245, 215, 70, a), cx, 30.f);
            }
            if (mState == GameState::Paused) {
                drawDimPanel(150);
                drawTextLine("PAUSED",       20, sf::Color::Cyan,  cx, 150.f);
                drawTextLine("ESC - RESUME", 10, sf::Color::White, cx, 188.f);
            }
            break;

        case GameState::GameOver:
            drawDimPanel(210);
            drawTextLine("GAME OVER",   24, sf::Color(230, 70, 70), cx, 135.f);
            drawTextLine("ENTER - MENU", 10, sf::Color::White,      cx, 195.f);
            break;

        case GameState::Win:
            drawDimPanel(210);
            drawTextLine("YOU WIN!",           22, sf::Color(90, 220, 120), cx, 96.f);
            drawTextLine("THE REALM IS SAVED", 10, gold,                    cx, 134.f);
            drawTextLine("TIME  " + formatTime(mPlayTime), 10,
                         sf::Color::White, cx, 168.f);
            drawTextLine("ENEMIES KILLED  " + std::to_string(mEnemiesKilled), 10,
                         sf::Color::White, cx, 188.f);
            drawTextLine("ENTER - MENU",       10, sf::Color::White,        cx, 224.f);
            break;
    }

    mWindow.display();
}

void Game::drawTextLine(const std::string& str, unsigned int size,
                        sf::Color colour, float centreX, float topY) {
    sf::Text text;
    text.setFont(mFont);
    text.setCharacterSize(size);
    text.setString(str);

    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.f, 0.f);

    text.setFillColor(sf::Color(0, 0, 0, 170));
    text.setPosition(centreX + 1.f, topY + 1.f);
    mWindow.draw(text);

    text.setFillColor(colour);
    text.setPosition(centreX, topY);
    mWindow.draw(text);
}

void Game::drawDimPanel(sf::Uint8 alpha) {
    sf::RectangleShape panel(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    panel.setFillColor(sf::Color(10, 8, 18, alpha));
    mWindow.draw(panel);
}

void Game::adjustVolume(int delta) {
    int& v = (mMenuSel == 0) ? mMusicVol : mSfxVol;
    v += delta;
    if (v < 0)   v = 0;
    if (v > 100) v = 100;

    if (mMenuSel == 0) {
        mMusic.setVolume(static_cast<float>(v));
    } else {
        mAudio.setMasterVolume(static_cast<float>(v));
        mAudio.play("hit");          // preview the new SFX level
    }
}

void Game::drawVolumeBar(const std::string& label, int value, float y,
                         bool selected) {
    sf::Color col = selected ? sf::Color(245, 215, 70)
                             : sf::Color(205, 205, 215);

    sf::Text text;
    text.setFont(mFont);
    text.setCharacterSize(9);
    text.setString((selected ? "> " : "  ") + label);
    text.setFillColor(sf::Color(0, 0, 0, 170));
    text.setPosition(101.f, y + 1.f);
    mWindow.draw(text);
    text.setFillColor(col);
    text.setPosition(100.f, y);
    mWindow.draw(text);

    float bx = 210.f, bw = 130.f, bh = 10.f;
    sf::RectangleShape back(sf::Vector2f(bw, bh));
    back.setPosition(bx, y);
    back.setFillColor(sf::Color(35, 35, 48));
    back.setOutlineThickness(1.f);
    back.setOutlineColor(col);
    mWindow.draw(back);

    sf::RectangleShape fill(sf::Vector2f(bw * value / 100.f, bh));
    fill.setPosition(bx, y);
    fill.setFillColor(col);
    mWindow.draw(fill);

    sf::Text pct;
    pct.setFont(mFont);
    pct.setCharacterSize(9);
    pct.setString(std::to_string(value));
    pct.setFillColor(col);
    pct.setPosition(bx + bw + 8.f, y);
    mWindow.draw(pct);
}

void Game::updateCamera() {
    if (!mPlayer)
        return;

    const sf::Vector2f vs = mWorldView.getSize();
    const float halfW = vs.x / 2.f;
    const float halfH = vs.y / 2.f;
    const float mapW  = mMap.getPixelWidth();
    const float mapH  = mMap.getPixelHeight();

    float cx = mPlayer->getX() + ENTITY_SIZE / 2.f;
    float cy = mPlayer->getY() + ENTITY_SIZE / 2.f;

    cx = (mapW <= vs.x) ? mapW / 2.f
                        : std::max(halfW, std::min(cx, mapW - halfW));
    cy = (mapH <= vs.y) ? mapH / 2.f
                        : std::max(halfH, std::min(cy, mapH - halfH));

    if (mShakeTimer > 0.f && mShakeDur > 0.f) {
        float k = (mShakeTimer / mShakeDur) * mShakeMag;
        cx += (std::rand() % 200 - 100) / 100.f * k;
        cy += (std::rand() % 200 - 100) / 100.f * k;
    }

    mWorldView.setCenter(std::round(cx), std::round(cy));
}

void Game::applyLetterbox() {
    sf::Vector2u ws = mWindow.getSize();
    float windowRatio = static_cast<float>(ws.x) / static_cast<float>(ws.y);
    float viewRatio   = static_cast<float>(WINDOW_WIDTH) /
                        static_cast<float>(WINDOW_HEIGHT);
    float sx = 1.f, sy = 1.f, px = 0.f, py = 0.f;
    if (windowRatio > viewRatio) {
        sx = viewRatio / windowRatio;
        px = (1.f - sx) / 2.f;
    } else {
        sy = windowRatio / viewRatio;
        py = (1.f - sy) / 2.f;
    }
    mUiView.setViewport(sf::FloatRect(px, py, sx, sy));
    mWorldView.setViewport(sf::FloatRect(px, py, sx, sy));
}

void Game::toggleFullscreen() {
    mFullscreen = !mFullscreen;
    if (mFullscreen)
        mWindow.create(sf::VideoMode::getDesktopMode(),
                       "Echoes of the Forgotten Realm", sf::Style::Fullscreen);
    else
        mWindow.create(sf::VideoMode(WINDOW_WIDTH * WINDOW_SCALE,
                                     WINDOW_HEIGHT * WINDOW_SCALE),
                       "Echoes of the Forgotten Realm",
                       sf::Style::Titlebar | sf::Style::Close);
    mWindow.setFramerateLimit(60);
    applyLetterbox();
}

void Game::triggerShake(float magnitude, float duration) {
    mShakeMag   = magnitude;
    mShakeDur   = duration;
    mShakeTimer = duration;
}

void Game::spawnBurst(float x, float y, sf::Color colour, int count) {
    for (int i = 0; i < count; ++i) {
        float ang = (std::rand() % 628) / 100.f;
        float spd = 35.f + static_cast<float>(std::rand() % 80);
        Particle p;
        p.x = x; p.y = y;
        p.vx = std::cos(ang) * spd;
        p.vy = std::sin(ang) * spd;
        p.maxLife = p.life = 0.35f + (std::rand() % 30) / 100.f;
        p.colour = colour;
        mParticles.push_back(p);
    }
}

void Game::updateParticles(float dt) {
    for (auto& p : mParticles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.vx *= 0.90f;
        p.vy *= 0.90f;
        p.life -= dt;
    }
    mParticles.erase(
        std::remove_if(mParticles.begin(), mParticles.end(),
            [](const Particle& p) { return p.life <= 0.f; }),
        mParticles.end());
}

void Game::drawParticles() {
    sf::RectangleShape dot(sf::Vector2f(3.f, 3.f));
    for (const auto& p : mParticles) {
        float a = p.life / p.maxLife;
        sf::Color c = p.colour;
        c.a = static_cast<sf::Uint8>(255 * std::max(0.f, a));
        dot.setPosition(p.x, p.y);
        dot.setFillColor(c);
        mWindow.draw(dot);
    }
}

std::string Game::formatTime(float seconds) const {
    int total = static_cast<int>(seconds);
    int m = total / 60;
    int s = total % 60;
    return std::to_string(m) + ":" + (s < 10 ? "0" : "") + std::to_string(s);
}

void Game::drawHearts() {
    const int maxHp   = mPlayer->getMaxHp();
    const int hp      = mPlayer->getHp();
    const int hearts  = (maxHp + 1) / 2;
    const float x0 = 6.f, y0 = 4.f;

    const sf::Texture* tex = mTextures.get("heart");
    if (tex) {
        sf::Sprite s(*tex);
        for (int i = 0; i < hearts; ++i) {
            int value = hp - i * 2;
            int frame = value >= 2 ? 2 : (value == 1 ? 1 : 0);
            s.setTextureRect(sf::IntRect(frame * 14, 0, 14, 14));
            s.setPosition(x0 + i * 15.f, y0);
            mWindow.draw(s);
        }
    } else {
        sf::Text t;
        t.setFont(mFont);
        t.setCharacterSize(8);
        t.setFillColor(sf::Color::White);
        t.setString("HP " + std::to_string(hp) + "/" + std::to_string(maxHp));
        t.setPosition(x0, y0 + 2.f);
        mWindow.draw(t);
    }
}
