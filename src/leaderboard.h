// Leaderboard System - High scores
#pragma once
#include <vector>
#include <string>

struct LeaderboardEntry {
    std::string playerName;
    int score;
    int waves;
    int kills;
    int rank;
};

class LeaderboardSystem {
public:
    std::vector<LeaderboardEntry> entries;
    int maxEntries;
    
    void init(int max);
    void addEntry(const std::string& name, int score, int waves, int kills);
    void sort();
    std::vector<LeaderboardEntry> getTop(int n);
};

void LeaderboardSystem::init(int max) {
    maxEntries = max;
}

void LeaderboardSystem::addEntry(const std::string& name, int score, int waves, int kills) {
    entries.push_back({name, score, waves, kills, 0});
    sort();
    if ((int)entries.size() > maxEntries) {
        entries.resize(maxEntries);
    }
}

void LeaderboardSystem::sort() {
    for (int i = 0; i < (int)entries.size(); i++) {
        for (int j = i + 1; j < (int)entries.size(); j++) {
            if (entries[j].score > entries[i].score) {
                LeaderboardEntry tmp = entries[i];
                entries[i] = entries[j];
                entries[j] = tmp;
            }
        }
    }
    for (int i = 0; i < (int)entries.size(); i++) {
        entries[i].rank = i + 1;
    }
}

std::vector<LeaderboardEntry> LeaderboardSystem::getTop(int n) {
    std::vector<LeaderboardEntry> result;
    for (int i = 0; i < n && i < (int)entries.size(); i++) {
        result.push_back(entries[i]);
    }
    return result;
}
