#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Constants.h"

// One parsed pickup request read from a level file: which item (type) and
// where (world-pixel top-left). Enemies are no longer parsed here — they are
// placed at random by the Game.
struct SpawnInfo {
    char  type;     // 'H' potion, 'R' rune shard, 'Y' key
    float x, y;
};

// TileMap — owns the level grid. It loads a level from a plain-text file,
// draws the tiles (with optional textures), answers collision queries against
// walls and the locked exit, and reports the player spawn, the pickups, and
// the list of free floor tiles used for random enemy placement. Encapsulation:
// the grid is private and the outside world only sees clean query methods.
class TileMap {
public:
    bool loadFromFile(const std::string& path);
    void draw(sf::RenderWindow& window) const;

    bool isSolid(float worldX, float worldY) const;
    bool isAreaSolid(const sf::FloatRect& bounds) const;

    sf::Vector2f getPlayerSpawn() const { return mPlayerSpawn; }
    const std::vector<SpawnInfo>& getSpawns() const { return mSpawns; }

    // Returns the top-left world pixel of every walkable floor tile. Used by
    // the game to choose RANDOM enemy spawn positions each time a level loads.
    std::vector<sf::Vector2f> getFreeFloorTiles() const;

    float getPixelWidth()  const { return static_cast<float>(mCols * TILE_SIZE); }
    float getPixelHeight() const { return static_cast<float>(mRows * TILE_SIZE); }

    void openExit() { mExitOpen = true; }
    bool isExitOpen() const { return mExitOpen; }
    sf::FloatRect getExitBounds() const;

    void setTextures(const sf::Texture* wall, const sf::Texture* floor,
                     const sf::Texture* exitClosed, const sf::Texture* exitOpen);

private:
    int tileAt(int col, int row) const;

    std::vector<std::vector<int>> mGrid;
    std::vector<SpawnInfo>        mSpawns;
    sf::Vector2f                  mPlayerSpawn{ 0.f, 0.f };
    sf::Vector2i                  mExitCell{ -1, -1 };
    bool                          mExitOpen = false;
    int                           mCols = 0;
    int                           mRows = 0;

    const sf::Texture* mExitOpenTex = nullptr;
    const sf::Texture* mWallTex  = nullptr;
    const sf::Texture* mFloorTex = nullptr;
    const sf::Texture* mExitTex  = nullptr;
};
