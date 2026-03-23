// PowerUp System - Collectible power-ups
#pragma once
#include <vector>
#include <string>

enum class PowerUpType {
    Health,
    Shield,
    Speed,
    Damage,
    invisibility,
    Nuke,
    Scanner,
    EMP
};

struct PowerUp {
    int x, y;
    PowerUpType type;
    float duration;
    bool active;
};

class PowerUpSystem {
public:
    std::vector<PowerUp> powerups;
    
    void spawn(int x, int y, PowerUpType type);
    void update(float dt);
    void collect(int x, int y);
    PowerUp* getAt(int x, int y);
};

void PowerUpSystem::spawn(int x, int y, PowerUpType type) {
    powerups.push_back({x, y, type, 30.0f, true});
}

void PowerUpSystem::update(float dt) {
    for (auto it = powerups.begin(); it != powerups.end(); ) {
        it->duration -= dt;
        if (it->duration <= 0 || !it->active) {
            it = powerups.erase(it);
        } else {
            ++it;
        }
    }
}

void PowerUpSystem::collect(int x, int y) {
    for (auto& p : powerups) {
        if (p.x == x && p.y == y && p.active) {
            p.active = false;
            break;
        }
    }
}

PowerUp* PowerUpSystem::getAt(int x, int y) {
    for (auto& p : powerups) {
        if (p.x == x && p.y == y && p.active) {
            return &p;
        }
    }
    return nullptr;
}
