// Sound System - Audio management
#pragma once
#include <string>
#include <map>

enum class SoundType {
    MUSIC,
    SFX,
    VOICE
};

class SoundSystem {
public:
    std::map<std::string, SoundType> sounds;
    float musicVolume;
    float sfxVolume;
    float voiceVolume;
    
    void init();
    void loadSound(const std::string& name, const std::string& file, SoundType type);
    void play(const std::string& name);
    void stop(const std::string& name);
    void setVolume(SoundType type, float volume);
};

void SoundSystem::init() {
    musicVolume = 0.8f;
    sfxVolume = 1.0f;
    voiceVolume = 0.9f;
    
    sounds = {
        {"background", SoundType::MUSIC},
        {"attack", SoundType::SFX},
        {"construct", SoundType::SFX},
        {"explosion", SoundType::SFX},
        {"select", SoundType::SFX},
        {"train", SoundType::SFX},
        {"wave_start", SoundType::SFX},
        {"victory", SoundType::MUSIC},
        {"defeat", SoundType::MUSIC}
    };
}

void SoundSystem::loadSound(const std::string& name, const std::string& file, SoundType type) {
    sounds[name] = type;
}

void SoundSystem::play(const std::string& name) {
    // Play sound
}

void SoundSystem::stop(const std::string& name) {
    // Stop sound
}

void SoundSystem::setVolume(SoundType type, float volume) {
    switch (type) {
        case SoundType::MUSIC: musicVolume = volume; break;
        case SoundType::SFX: sfxVolume = volume; break;
        case SoundType::VOICE: voiceVolume = volume; break;
    }
}
