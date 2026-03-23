// Campaign System - Story missions
#pragma once
#include <vector>
#include <string>

struct Mission {
    int id;
    std::string name;
    std::string briefing;
    std::string objective;
    int rewardMinerals;
    int rewardXP;
    int difficulty;
    bool unlocked;
    bool completed;
};

class CampaignSystem {
public:
    std::vector<Mission> missions;
    int currentMission;
    
    void init();
    bool startMission(int id);
    void completeMission(int id);
    void failMission(int id);
    Mission* getMission(int id);
    int getTotalStars();
};

void CampaignSystem::init() {
    missions = {
        {1, "First Contact", "The enemy approaches. Build defenses!", "Survive wave 3", 200, 100, 1, true, false},
        {2, "Resource Rush", "Gather resources for the war effort.", "Collect 2000 minerals", 300, 150, 1, false, false},
        {3, "Scorched Earth", "Destroy the enemy outpost.", "Destroy 5 enemy buildings", 400, 200, 2, false, false},
        {4, "The Line", "Hold the line against the assault!", "Survive wave 5", 500, 250, 2, false, false},
        {5, "Deep Recon", "Explore enemy territory.", "Reveal the map", 350, 175, 2, false, false},
        {6, "Iron Curtain", "Fortify defenses.", "Build 3 turrets and survive", 600, 300, 3, false, false},
        {7, "Death from Above", "Launch an air assault.", "Destroy enemy base with Fighters", 800, 400, 3, false, false},
        {8, "The Gauntlet", "Final assault on the stronghold.", "Defeat the boss", 1000, 500, 4, false, false},
        {9, "Extraction", "Rescue the hostages.", "Keep Commander alive", 750, 375, 3, false, false},
        {10, "Last Stand", "This is our final defense!", "Survive 10 waves", 1200, 600, 4, false, false}
    };
    currentMission = 0;
}

bool CampaignSystem::startMission(int id) {
    for (auto& m : missions) {
        if (m.id == id && m.unlocked && !m.completed) {
            currentMission = id;
            return true;
        }
    }
    return false;
}

void CampaignSystem::completeMission(int id) {
    for (auto& m : missions) {
        if (m.id == id) {
            m.completed = true;
            if (id + 1 < (int)missions.size()) {
                missions[id].unlocked = true;
            }
        }
    }
}

void CampaignSystem::failMission(int id) {
    // Mission failed, can retry
}

Mission* CampaignSystem::getMission(int id) {
    for (auto& m : missions) {
        if (m.id == id) return &m;
    }
    return nullptr;
}

int CampaignSystem::getTotalStars() {
    int stars = 0;
    for (const auto& m : missions) {
        if (m.completed) stars++;
    }
    return stars;
}
