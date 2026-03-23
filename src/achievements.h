// Achievement System - Track player accomplishments
#pragma once
#include <vector>
#include <string>

struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    int rewardXP;
    bool unlocked;
    float progress;
    float target;
};

class AchievementSystem {
public:
    std::vector<Achievement> achievements;
    
    void init();
    void unlock(const std::string& id);
    void addProgress(const std::string& id, float amount);
    std::vector<Achievement> getUnlocked();
    std::vector<Achievement> getLocked();
};

void AchievementSystem::init() {
    achievements = {
        {"first_blood", "First Blood", "Defeat your first enemy", 50, false, 0, 1},
        {"builder", "Master Builder", "Build 10 structures", 100, false, 0, 10},
        {"soldier", "Soldier", "Train 10 Marines", 75, false, 0, 10},
        {"gatherer", "Resource Gatherer", "Collect 5000 minerals", 50, false, 0, 5000},
        {"explorer", "Explorer", "Discover 100 tiles", 60, false, 0, 100},
        {"defender", "Defender", "Survive 5 waves", 80, false, 0, 5},
        {"destroyer", "Base Destroyer", "Destroy 3 enemy bases", 200, false, 0, 3},
        {"tanker", "Heavy Metal", "Build 3 Tanks", 150, false, 0, 3},
        {"veteran", "Veteran", "Play for 30 minutes", 100, false, 0, 1800},
        {"rich", "Wealthy", "Accumulate 10000 minerals", 250, false, 0, 10000},
        {"killer", "Serial Killer", "Defeat 100 enemies", 300, false, 0, 100},
        {"boss_slayer", "Boss Slayer", "Defeat a boss enemy", 500, false, 0, 1},
        {"speed_demon", "Speed Demon", "Complete a wave in under 30 seconds", 100, false, 0, 1},
        {"unstoppable", "Unstoppable", "Win 10 games", 400, false, 0, 10},
        {"technician", "Tech Expert", "Research all technologies", 200, false, 0, 5},
        {"zealot_slayer", "Zealot Slayer", "Defeat 50 Zerg units", 150, false, 0, 50},
        {"carrier_bane", "Carrier Bane", "Destroy 5 Carriers", 300, false, 0, 5},
        {"airborne", "Airborne", "Build 10 Fighters", 100, false, 0, 10},
        {"ground_pounder", "Ground Pounder", "Build 10 Tanks", 150, false, 0, 10},
        {"economist", "Economist", "Gather 50000 minerals in one game", 300, false, 0, 50000},
        {"perfectionist", "Perfectionist", "Win without losing any buildings", 400, false, 0, 1},
        {"speedrunner", "Speedrunner", "Complete wave 10 in under 5 minutes", 350, false, 0, 1},
        {"survivor", "Survivor", "Survive 20 waves", 500, false, 0, 20},
        {"commander", "Supreme Commander", "Reach wave 25", 1000, false, 0, 25}
    };
}

void AchievementSystem::unlock(const std::string& id) {
    for (auto& a : achievements) {
        if (a.id == id) {
            a.unlocked = true;
            a.progress = a.target;
        }
    }
}

void AchievementSystem::addProgress(const std::string& id, float amount) {
    for (auto& a : achievements) {
        if (a.id == id && !a.unlocked) {
            a.progress += amount;
            if (a.progress >= a.target) {
                unlock(id);
            }
        }
    }
}

std::vector<Achievement> AchievementSystem::getUnlocked() {
    std::vector<Achievement> result;
    for (const auto& a : achievements) {
        if (a.unlocked) result.push_back(a);
    }
    return result;
}

std::vector<Achievement> AchievementSystem::getLocked() {
    std::vector<Achievement> result;
    for (const auto& a : achievements) {
        if (!a.unlocked) result.push_back(a);
    }
    return result;
}
