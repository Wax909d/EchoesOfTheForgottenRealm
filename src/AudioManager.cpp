#include "AudioManager.h"
#include <cstdlib>

AudioManager::AudioManager() {

    mVoices.resize(8);
}

static float randomPitch() {
    return 0.92f + static_cast<float>(std::rand() % 16) / 100.f;
}

bool AudioManager::loadSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path))
        return false;
    mBuffers[name] = std::move(buffer);
    return true;
}

void AudioManager::play(const std::string& name) {
    auto it = mBuffers.find(name);
    if (it == mBuffers.end())
        return;

    for (auto& voice : mVoices) {
        if (voice.getStatus() != sf::Sound::Playing) {
            voice.setBuffer(it->second);
            voice.setPitch(randomPitch());
            voice.setVolume(mMaster);
            voice.play();
            return;
        }
    }
    mVoices.front().setBuffer(it->second);
    mVoices.front().setPitch(randomPitch());
    mVoices.front().setVolume(mMaster);
    mVoices.front().play();
}
