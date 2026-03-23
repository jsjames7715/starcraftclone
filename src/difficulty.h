// Difficulty System - Game difficulty settings
#pragma once
#include <string>

enum class Difficulty {
    Easy,
    Normal,
    Hard,
    Brutal,
    Insane
};

struct DifficultySettings {
    Difficulty level;
    float enemySpawnRate;
    float enemyDamageMultiplier;
    float enemyHealthMultiplier;
    float resourceMultiplier;
    int startingMinerals;
    int startingWave;
    bool enemyTanksEnabled;
    bool enemyAirEnabled;
};

class DifficultySystem {
public:
    DifficultySettings currentDifficulty;
    
    void setDifficulty(Difficulty level);
    DifficultySettings getSettings();
    std::string getDifficultyName();
};

void DifficultySystem::setDifficulty(Difficulty level) {
    currentDifficulty.level = level;
    switch (level) {
        case Difficulty::Easy:
            currentDifficulty = {level, 0.7f, 0.7f, 0.7f, 1.5f, 1000, 1, false, false};
            break;
        case Difficulty::Normal:
            currentDifficulty = {level, 1.0f, 1.0f, 1.0f, 1.0f, 500, 1, true, true};
            break;
        case Difficulty::Hard:
            currentDifficulty = {level, 1.3f, 1.3f, 1.3f, 0.8f, 300, 2, true, true};
            break;
        case Difficulty::Brutal:
            currentDifficulty = {level, 1.6f, 1.6f, 1.6f, 0.5f, 200, 3, true, true};
            break;
        case Difficulty::Insane:
            currentDifficulty = {level, 2.0f, 2.0f, 2.0f, 0.3f, 100, 5, true, true};
            break;
    }
}

DifficultySettings DifficultySystem::getSettings() {
    return currentDifficulty;
}

std::string DifficultySystem::getDifficultyName() {
    switch (currentDifficulty.level) {
        case Difficulty::Easy: return "Easy";
        case Difficulty::Normal: return "Normal";
        case Difficulty::Hard: return "Hard";
        case Difficulty::Brutal: return "Brutal";
        case Difficulty::Insane: return "Insane";
    }
    return "Unknown";
}
