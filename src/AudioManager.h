#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <vector>
#include <string>

class AudioManager {
public:
    AudioManager();

    bool loadSound(const std::string& name, const std::string& path);
    void play(const std::string& name);

    void  setMasterVolume(float v) { mMaster = v; }
    float getMasterVolume() const { return mMaster; }

private:
    std::map<std::string, sf::SoundBuffer> mBuffers;
    std::vector<sf::Sound>                 mVoices;
    float                                  mMaster = 100.f;
};
