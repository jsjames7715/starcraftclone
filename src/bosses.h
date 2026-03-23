// Boss System - Special boss enemies
#pragma once
#include <vector>
#include <string>

struct Boss {
    std::string name;
    int health;
    int maxHealth;
    int damage;
    int x, y;
    int rewardMinerals;
    int rewardXP;
    std::vector<std::string> abilities;
    bool spawned;
    bool defeated;
};

class BossSystem {
public:
    std::vector<Boss> bosses;
    Boss* currentBoss;
    int bossWaveInterval;
    int currentWave;
    
    void init();
    void update();
    void spawnBoss(int wave);
    void damageBoss(int damage);
    Boss* getCurrentBoss();
    bool isBossActive();
};

void BossSystem::init() {
    bossWaveInterval = 5;
    currentWave = 0;
    currentBoss = nullptr;
    
    bosses = {
        {"Overlord", 1000, 1000, 30, 0, 0, 500, 200, {"spawn_zerglings", "enrage"}, false, false},
        {"Siege Tank Commander", 1500, 1500, 60, 0, 0, 600, 300, {"siege_mode", "artillery_barrage"}, false, false},
        {"Carrier Flagship", 2000, 2000, 40, 0, 0, 800, 400, {"launch_interceptors", "massRecall"}, false, false},
        {"Ultralisk Alpha", 2500, 2500, 80, 0, 0, 1000, 500, {"charge", "spawn_chromosomes"}, false, false},
        {"Wraith Ace", 1200, 1200, 50, 0, 0, 700, 350, {"cloak", "missile_storm"}, false, false},
        {"Battlecruiser", 3000, 3000, 100, 0, 0, 1500, 750, {"yamato_cannon", "defensive_matrix"}, false, false},
        {"Devourer", 1800, 1800, 45, 0, 0, 900, 450, {"corrosive_acid", "spawn_broodlings"}, false, false},
        {"Terran General", 2000, 2000, 55, 0, 0, 1000, 500, {"commander_aura", "reinforcements"}, false, false}
    };
}

void BossSystem::update() {
    currentWave++;
    if (currentWave % bossWaveInterval == 0) {
        spawnBoss(currentWave);
    }
}

void BossSystem::spawnBoss(int wave) {
    if (wave / bossWaveInterval - 1 < (int)bosses.size()) {
        Boss& boss = bosses[wave / bossWaveInterval - 1];
        boss.spawned = true;
        boss.defeated = false;
        boss.x = 275;
        boss.y = 275;
        boss.health = boss.maxHealth;
        currentBoss = &boss;
    }
}

void BossSystem::damageBoss(int damage) {
    if (currentBoss) {
        currentBoss->health -= damage;
        if (currentBoss->health <= 0) {
            currentBoss->defeated = true;
            currentBoss = nullptr;
        }
    }
}

Boss* BossSystem::getCurrentBoss() {
    return currentBoss;
}

bool BossSystem::isBossActive() {
    return currentBoss != nullptr;
}
