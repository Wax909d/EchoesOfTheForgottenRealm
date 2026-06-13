#include "TextureManager.h"

bool TextureManager::load(const std::string& name, const std::string& path) {
    sf::Texture tex;
    if (!tex.loadFromFile(path))
        return false;
    tex.setSmooth(false);
    mTextures[name] = std::move(tex);
    return true;
}

const sf::Texture* TextureManager::get(const std::string& name) const {
    auto it = mTextures.find(name);
    return it == mTextures.end() ? nullptr : &it->second;
}
