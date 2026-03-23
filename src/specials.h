// SpecialAbilities System - Unit special abilities
#pragma once
#include <vector>
#include <string>

struct Ability {
    std::string name;
    int cooldown;
    int currentCooldown;
    int energyCost;
    bool unlocked;
};

class SpecialAbilities {
public:
    std::vector<Ability> abilities;
    
    void init();
    bool useAbility(int index);
    void update(float dt);
    void unlock(int index);
};

void SpecialAbilities::init() {
    abilities = {
        {"Stim Pack", 30, 0, 20, true},
        {"Heal", 45, 0, 30, true},
        {"Cloaking Field", 60, 0, 40, false},
        {"Nuclear Strike", 180, 0, 100, false},
        {"Scanner Sweep", 20, 0, 10, true},
        {"Defensive Matrix", 90, 0, 50, false},
        {"Psionic Storm", 120, 0, 75, false},
        {"Lock Down", 150, 0, 80, false},
        {"EMP Shockwave", 90, 0, 60, false},
        {"Yamato Cannon", 200, 0, 100, false}
    };
}

bool SpecialAbilities::useAbility(int index) {
    if (index >= 0 && index < (int)abilities.size()) {
        Ability& a = abilities[index];
        if (a.currentCooldown <= 0 && a.unlocked) {
            a.currentCooldown = a.cooldown;
            return true;
        }
    }
    return false;
}

void SpecialAbilities::update(float dt) {
    for (auto& a : abilities) {
        if (a.currentCooldown > 0) {
            a.currentCooldown -= dt;
            if (a.currentCooldown < 0) a.currentCooldown = 0;
        }
    }
}

void SpecialAbilities::unlock(int index) {
    if (index >= 0 && index < (int)abilities.size()) {
        abilities[index].unlocked = true;
    }
}
