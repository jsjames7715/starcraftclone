// UpgradeSystem - Technology upgrades
#pragma once
#include <vector>
#include <string>

struct Upgrade {
    int id;
    std::string name;
    std::string description;
    int mineralCost;
    int gasCost;
    float researchTime;
    bool researched;
    bool inProgress;
    float progress;
};

class UpgradeSystem {
public:
    std::vector<Upgrade> upgrades;
    
    void init();
    bool startResearch(int id);
    void update(float dt);
    void completeResearch(int id);
    Upgrade* getUpgrade(int id);
};

void UpgradeSystem::init() {
    upgrades = {
        {1, "Weapons Level 1", "+10% damage", 100, 50, 60, false, false, 0},
        {2, "Weapons Level 2", "+20% damage", 200, 100, 120, false, false, 0},
        {3, "Weapons Level 3", "+30% damage", 300, 200, 180, false, false, 0},
        {4, "Armor Level 1", "+1 armor", 100, 50, 60, false, false, 0},
        {5, "Armor Level 2", "+2 armor", 200, 100, 120, false, false, 0},
        {6, "Armor Level 3", "+3 armor", 300, 200, 180, false, false, 0},
        {7, "Stim Pack", "Marines use stim", 150, 100, 90, false, false, 0},
        {8, "Siege Mode", "Tanks siege ability", 200, 150, 120, false, false, 0},
        {9, "Cloaking", "Wraiths cloak", 300, 200, 150, false, false, 0},
        {10, "Jet Packs", "Marines fly", 250, 150, 120, false, false, 0},
        {11, "Healing Upgrade", "+50% heal rate", 150, 100, 90, false, false, 0},
        {12, "Tank Treads", "+20% tank speed", 200, 100, 100, false, false, 0},
        {13, "Carrier Launch", "Carriers launch interceptors", 350, 250, 200, false, false, 0},
        {14, "Dark Templar", "Unlock Dark Templar", 400, 300, 180, false, false, 0},
        {15, "Nuclear Launch", "Unlock nuclear", 500, 400, 240, false, false, 0}
    };
}

bool UpgradeSystem::startResearch(int id) {
    Upgrade* upg = getUpgrade(id);
    if (upg && !upg->researched && !upg->inProgress) {
        upg->inProgress = true;
        upg->progress = 0;
        return true;
    }
    return false;
}

void UpgradeSystem::update(float dt) {
    for (auto& upg : upgrades) {
        if (upg.inProgress && !upg.researched) {
            upg.progress += dt;
            if (upg.progress >= upg.researchTime) {
                completeResearch(upg.id);
            }
        }
    }
}

void UpgradeSystem::completeResearch(int id) {
    Upgrade* upg = getUpgrade(id);
    if (upg) {
        upg->researched = true;
        upg->inProgress = false;
    }
}

Upgrade* UpgradeSystem::getUpgrade(int id) {
    for (auto& upg : upgrades) {
        if (upg.id == id) return &upg;
    }
    return nullptr;
}
