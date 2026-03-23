// Statistics System - Track player stats
#pragma once
#include <string>

struct PlayerStats {
    std::string playerName;
    int totalGames;
    int gamesWon;
    int gamesLost;
    int totalKills;
    int totalDeaths;
    int buildingsBuilt;
    int buildingsDestroyed;
    int resourcesGathered;
    int wavesSurvived;
    float totalPlayTime;
    int highestWave;
    int mostKillsInGame;
};

class StatsSystem {
public:
    PlayerStats stats;
    
    void init(const std::string& name);
    void recordGame(bool won, int kills, int deaths, int wave);
    void update(int kills, int deaths, int built, int destroyed, int resources, int waves);
    int getWinRate();
    float getKDRatio();
};

void StatsSystem::init(const std::string& name) {
    stats = {name, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0, 0};
}

void StatsSystem::recordGame(bool won, int kills, int deaths, int wave) {
    stats.totalGames++;
    if (won) stats.gamesWon++;
    else stats.gamesLost++;
    
    if (kills > stats.mostKillsInGame) stats.mostKillsInGame = kills;
    if (wave > stats.highestWave) stats.highestWave = wave;
}

void StatsSystem::update(int kills, int deaths, int built, int destroyed, int resources, int waves) {
    stats.totalKills += kills;
    stats.totalDeaths += deaths;
    stats.buildingsBuilt += built;
    stats.buildingsDestroyed += destroyed;
    stats.resourcesGathered += resources;
    stats.wavesSurvived += waves;
}

int StatsSystem::getWinRate() {
    if (stats.totalGames == 0) return 0;
    return (stats.gamesWon * 100) / stats.totalGames;
}

float StatsSystem::getKDRatio() {
    if (stats.totalDeaths == 0) return stats.totalKills;
    return (float)stats.totalKills / stats.totalDeaths;
}
