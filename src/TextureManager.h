#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class TextureManager {
public:

    bool load(const std::string& name, const std::string& path);

    const sf::Texture* get(const std::string& name) const;

private:
    std::map<std::string, sf::Texture> mTextures;
};
