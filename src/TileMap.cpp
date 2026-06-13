#include "TileMap.h"
#include "Constants.h"
#include <fstream>

bool TileMap::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    mGrid.clear();
    mSpawns.clear();
    mExitCell    = { -1, -1 };
    mExitOpen    = false;
    mPlayerSpawn = { TILE_SIZE * 1.f, TILE_SIZE * 1.f };

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
        lines.push_back(line);

    mRows = static_cast<int>(lines.size());
    mCols = 0;
    for (const auto& l : lines)
        mCols = std::max(mCols, static_cast<int>(l.size()));

    mGrid.assign(mRows, std::vector<int>(mCols, 0));

    for (int row = 0; row < mRows; ++row) {
        const std::string& l = lines[row];
        for (int col = 0; col < mCols; ++col) {
            char c = (col < static_cast<int>(l.size())) ? l[col] : '.';

            float px = static_cast<float>(col * TILE_SIZE);
            float py = static_cast<float>(row * TILE_SIZE);

            switch (c) {
                case '#':
                case '=':
                    mGrid[row][col] = 1;
                    break;

                case 'E':
                    mGrid[row][col] = 2;
                    mExitCell = { col, row };
                    break;

                case 'P':
                    mPlayerSpawn = { px, py };
                    break;

                // Maps only define pickups now; enemies are placed randomly.
                case 'H': case 'R': case 'Y':   // potion, rune shard, key
                    mSpawns.push_back({ c, px, py });
                    break;

                case '.':
                case ' ':
                default:
                    mGrid[row][col] = 0;
                    break;
            }
        }
    }
    return true;
}

// Collect every walkable floor cell as a world-pixel position. Grid value 0
// is floor; 1 is wall and 2 is the exit, both of which are skipped.
std::vector<sf::Vector2f> TileMap::getFreeFloorTiles() const {
    std::vector<sf::Vector2f> out;
    for (int row = 0; row < mRows; ++row)
        for (int col = 0; col < mCols; ++col)
            if (mGrid[row][col] == 0)
                out.emplace_back(static_cast<float>(col * TILE_SIZE),
                                 static_cast<float>(row * TILE_SIZE));
    return out;
}

int TileMap::tileAt(int col, int row) const {
    if (col < 0 || row < 0 || col >= mCols || row >= mRows)
        return 1;
    return mGrid[row][col];
}

bool TileMap::isSolid(float worldX, float worldY) const {
    int col = static_cast<int>(worldX) / TILE_SIZE;
    int row = static_cast<int>(worldY) / TILE_SIZE;
    int t   = tileAt(col, row);

    if (t == 1) return true;
    if (t == 2) return !mExitOpen;
    return false;
}

bool TileMap::isAreaSolid(const sf::FloatRect& b) const {

    return isSolid(b.left,               b.top) ||
           isSolid(b.left + b.width - 1, b.top) ||
           isSolid(b.left,               b.top + b.height - 1) ||
           isSolid(b.left + b.width - 1, b.top + b.height - 1);
}

sf::FloatRect TileMap::getExitBounds() const {
    if (mExitCell.x < 0)
        return sf::FloatRect();
    return sf::FloatRect(
        static_cast<float>(mExitCell.x * TILE_SIZE),
        static_cast<float>(mExitCell.y * TILE_SIZE),
        static_cast<float>(TILE_SIZE),
        static_cast<float>(TILE_SIZE));
}

void TileMap::setTextures(const sf::Texture* wall, const sf::Texture* floor,
                          const sf::Texture* exitClosed, const sf::Texture* exitOpen) {
    mWallTex     = wall;
    mFloorTex    = floor;
    mExitTex     = exitClosed;
    mExitOpenTex = exitOpen;
}

void TileMap::draw(sf::RenderWindow& window) const {
    sf::RectangleShape cell(sf::Vector2f(TILE_SIZE - 1.f, TILE_SIZE - 1.f));
    sf::Sprite sprite;

    for (int row = 0; row < mRows; ++row) {
        for (int col = 0; col < mCols; ++col) {
            int   t   = mGrid[row][col];
            float px  = static_cast<float>(col * TILE_SIZE);
            float py  = static_cast<float>(row * TILE_SIZE);

            if (t == 2) {
                if (mFloorTex) {
                    sprite.setTexture(*mFloorTex, true);
                    sprite.setColor(sf::Color::White);
                    sprite.setPosition(px, py);
                    window.draw(sprite);
                }
                const sf::Texture* door = mExitOpen ? mExitOpenTex : mExitTex;
                if (door) {
                    sprite.setTexture(*door, true);
                    sprite.setColor(sf::Color::White);
                    sprite.setPosition(px, py);
                    window.draw(sprite);
                } else if (!mFloorTex) {
                    cell.setPosition(px, py);
                    cell.setFillColor(mExitOpen ? sf::Color(70, 210, 110)
                                                : sf::Color(95, 65, 35));
                    window.draw(cell);
                }
                continue;
            }

            const sf::Texture* tex = (t == 1) ? mWallTex : mFloorTex;
            if (tex) {
                sprite.setTexture(*tex, true);
                sprite.setColor(sf::Color::White);
                sprite.setPosition(px, py);
                window.draw(sprite);
            } else {
                cell.setPosition(px, py);
                cell.setFillColor(t == 1 ? sf::Color(45, 45, 65)
                                         : sf::Color(26, 26, 38));
                window.draw(cell);
            }
        }
    }
}
