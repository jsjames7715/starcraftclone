// Advanced terminal-based StarCraft-style RTS prototype using ncurses
// Compile with: g++ -std=c++17 -lncurses -o starcraft src/main.cpp
// Controls:
//   Arrow keys / WASD - move camera
//   s - spawn worker at command centre
//   t - train soldier (requires barracks)
//   b - build barracks (near command centre)
//   c - build command centre (costs 400 minerals)
//   m - move selected unit to cursor
//   a - attack with selected unit
//   r - repair selected building
//   q - quit
// Features: multiple unit types, buildings, combat, tech tree, fog of war

#include <ncurses.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

struct Vec2 { int x, y; Vec2(int X=0,int Y=0):x(X),y(Y){} Vec2 operator+(const Vec2& o)const{return Vec2(x+o.x,y+o.y);} Vec2 operator-(const Vec2& o)const{return Vec2(x-o.x,y-o.y);} int dist(const Vec2& o)const{return std::abs(x-o.x)+std::abs(y-o.y);} bool operator==(const Vec2& o)const{return x==o.x&&y==o.y;} bool operator!=(const Vec2& o)const{return x!=o.x||y!=o.y;} };
struct Vec3 { float x,y,z; Vec3(float X=0,float Y=0,float Z=0):x(X),y(Y),z(Z){} Vec3 normalized()const{float l=std::sqrt(x*x+y*y+z*z);return l?Vec3(x/l,y/l,z/l):Vec3();} };

enum class Team { Player, Enemy, Neutral };

enum class UnitType { 
    Worker, Marine, Medic, Tank, Fighter, Zergling, Ultralisk,
    Ghost, Vulture, Goliath, Wraith, Battlecruiser, ScienceVessel,
    Firebat, Dropship, Valkyrie, Corsair, Scout, DarkTemplar,
    Dragoon, Zealot, Archon, Carrier, Reaver, Shuttle
};

enum class BuildingType {
    CommandCentre, Barracks, Factory, Starport, SupplyDepot, Refinery,
    EnemyBase, EnemyBarracks, Turret, Bunker, Academy, Armory, TechLab,
    ComSat, NuclearSilo, SpiderMine, Pylon, Gateway, CyberneticsCore,
    Stargate, FleetBeacon, RoboticsBay, Observatory
};

enum class GameState { Playing, Paused, GameOver, Victory, Menu };
enum class GameMode { Campaign, Skirmish, Deathmatch, Survival, Rush, Economic, TowerDefense, CaptureTheFlag, KingOfTheHill, Custom };

const int MAP_W = 300;
const int MAP_H = 300;
const int VISION_RANGE = 8;
const int ATTACK_RANGE_MELEE = 1;
const int ATTACK_RANGE_RANGED = 5;

struct Resource {
    int minerals = 500;
    int gas = 0;
    int supplyUsed = 0;
    int supplyMax = 10;
};

struct Unit {
    Vec2 pos;
    Vec2 targetPos;
    UnitType type;
    Team team;
    bool selected = false;
    bool alive = true;
    int health = 100;
    int maxHealth = 100;
    int attackPower = 10;
    int armor = 0;
    float attackCooldown = 0.0f;
    float moveProgress = 0.0f;
    
    Unit(Vec2 p, UnitType t, Team tm):pos(p),type(t),team(tm),targetPos(p) {
        switch(t) {
            case UnitType::Worker: maxHealth=health=60; attackPower=5; armor=0; break;
            case UnitType::Marine: maxHealth=health=80; attackPower=12; armor=1; break;
            case UnitType::Medic: maxHealth=health=60; attackPower=0; armor=0; break;
            case UnitType::Tank: maxHealth=health=200; attackPower=50; armor=3; break;
            case UnitType::Fighter: maxHealth=health=100; attackPower=25; armor=2; break;
            case UnitType::Zergling: maxHealth=health=40; attackPower=8; armor=0; break;
            case UnitType::Ultralisk: maxHealth=health=400; attackPower=40; armor=5; break;
            case UnitType::Ghost: maxHealth=health=90; attackPower=20; armor=1; break;
            case UnitType::Vulture: maxHealth=health=80; attackPower=20; armor=0; break;
            case UnitType::Goliath: maxHealth=health=150; attackPower=30; armor=2; break;
            case UnitType::Wraith: maxHealth=health=120; attackPower=25; armor=2; break;
            case UnitType::Battlecruiser: maxHealth=health=500; attackPower=60; armor=5; break;
            case UnitType::ScienceVessel: maxHealth=health=200; attackPower=0; armor=2; break;
            case UnitType::Firebat: maxHealth=health=100; attackPower=16; armor=1; break;
            case UnitType::Dropship: maxHealth=health=150; attackPower=0; armor=2; break;
            case UnitType::Valkyrie: maxHealth=health=180; attackPower=35; armor=3; break;
            case UnitType::Corsair: maxHealth=health=100; attackPower=18; armor=1; break;
            case UnitType::Scout: maxHealth=health=80; attackPower=15; armor=0; break;
            case UnitType::DarkTemplar: maxHealth=health=80; attackPower=45; armor=1; break;
            case UnitType::Dragoon: maxHealth=health=180; attackPower=30; armor=2; break;
            case UnitType::Zealot: maxHealth=health=160; attackPower=20; armor=1; break;
            case UnitType::Archon: maxHealth=health=350; attackPower=50; armor=3; break;
            case UnitType::Carrier: maxHealth=health=400; attackPower=40; armor=4; break;
            case UnitType::Reaver: maxHealth=health=200; attackPower=100; armor=2; break;
            case UnitType::Shuttle: maxHealth=health=120; attackPower=0; armor=2; break;
        }
    }
    
    char glyph() const {
        if (!alive) return '%';
        if (team==Team::Player) {
            switch(type) {
                case UnitType::Worker: return 'w';
                case UnitType::Marine: return 'm';
                case UnitType::Medic: return 'M';
                case UnitType::Tank: return 'T';
                case UnitType::Fighter: return 'f';
                case UnitType::Zergling: return 'z';
                case UnitType::Ultralisk: return 'U';
                case UnitType::Ghost: return 'g';
                case UnitType::Vulture: return 'v';
                case UnitType::Goliath: return 'G';
                case UnitType::Wraith: return 'W';
                case UnitType::Battlecruiser: return 'B';
                case UnitType::ScienceVessel: return 'V';
                case UnitType::Firebat: return 'F';
                case UnitType::Dropship: return 'D';
                case UnitType::Valkyrie: return 'A';
                case UnitType::Corsair: return 'C';
                case UnitType::Scout: return 'S';
                case UnitType::DarkTemplar: return 'D';
                case UnitType::Dragoon: return 'd';
                case UnitType::Zealot: return 'Z';
                case UnitType::Archon: return 'A';
                case UnitType::Carrier: return 'c';
                case UnitType::Reaver: return 'R';
                case UnitType::Shuttle: return 'h';
            }
        } else {
            switch(type) {
                case UnitType::Worker: return 'W';
                case UnitType::Marine: return 'S';
                case UnitType::Tank: return 'K';
                case UnitType::Fighter: return 'F';
                case UnitType::Zergling: return 'Z';
                case UnitType::Ultralisk: return 'X';
                case UnitType::Ghost: return 's';
                case UnitType::Vulture: return 't';
                case UnitType::Goliath: return 'y';
                case UnitType::Wraith: return 'r';
                case UnitType::Battlecruiser: return 'b';
                case UnitType::ScienceVessel: return 'n';
                case UnitType::Firebat: return 'f';
                case UnitType::Dropship: return 'd';
                case UnitType::Valkyrie: return 'a';
                case UnitType::Corsair: return 'c';
                case UnitType::Scout: return 's';
                case UnitType::DarkTemplar: return 'x';
                case UnitType::Dragoon: return 'D';
                case UnitType::Zealot: return 'z';
                case UnitType::Archon: return 'a';
                case UnitType::Carrier: return 'C';
                case UnitType::Reaver: return 'e';
                case UnitType::Shuttle: return 'H';
            }
        }
        return '?';
    }
    
    int cost() const {
        switch(type) {
            case UnitType::Worker: return 50;
            case UnitType::Marine: return 100;
            case UnitType::Medic: return 100;
            case UnitType::Tank: return 300;
            case UnitType::Fighter: return 250;
            case UnitType::Zergling: return 50;
            case UnitType::Ultralisk: return 400;
            case UnitType::Ghost: return 200;
            case UnitType::Vulture: return 150;
            case UnitType::Goliath: return 250;
            case UnitType::Wraith: return 200;
            case UnitType::Battlecruiser: return 600;
            case UnitType::ScienceVessel: return 350;
            case UnitType::Firebat: return 150;
            case UnitType::Dropship: return 250;
            case UnitType::Valkyrie: return 350;
            case UnitType::Corsair: return 200;
            case UnitType::Scout: return 100;
            case UnitType::DarkTemplar: return 250;
            case UnitType::Dragoon: return 200;
            case UnitType::Zealot: return 150;
            case UnitType::Archon: return 400;
            case UnitType::Carrier: return 550;
            case UnitType::Reaver: return 400;
            case UnitType::Shuttle: return 200;
        }
        return 100;
    }
    
    int buildTime() const { return cost() / 10; }
};

struct Building {
    Vec2 pos;
    Vec2 size;
    BuildingType type;
    Team team;
    bool selected = false;
    bool alive = true;
    int health = 500;
    int maxHealth = 500;
    float buildProgress = 0.0f;
    bool underConstruction = false;
    
    Building(Vec2 p, BuildingType t, Team tm, Vec2 s=Vec2(3,3)):pos(p),type(t),team(tm),size(s) {
        switch(t) {
            case BuildingType::CommandCentre: maxHealth=health=1500; break;
            case BuildingType::Barracks: maxHealth=health=800; break;
            case BuildingType::Factory: maxHealth=health=1000; break;
            case BuildingType::Starport: maxHealth=health=700; break;
            case BuildingType::SupplyDepot: maxHealth=health=500; break;
            case BuildingType::Refinery: maxHealth=health=600; break;
            case BuildingType::EnemyBase: maxHealth=health=2000; break;
            case BuildingType::EnemyBarracks: maxHealth=health=1000; break;
            case BuildingType::Turret: maxHealth=health=400; size=Vec2(1,1); break;
            case BuildingType::Bunker: maxHealth=health=600; break;
            case BuildingType::Academy: maxHealth=health=700; break;
            case BuildingType::Armory: maxHealth=health=800; break;
            case BuildingType::TechLab: maxHealth=health=500; break;
            case BuildingType::ComSat: maxHealth=health=500; break;
            case BuildingType::NuclearSilo: maxHealth=health=600; break;
            case BuildingType::Pylon: maxHealth=health=300; size=Vec2(2,2); break;
            case BuildingType::Gateway: maxHealth=health=800; break;
            case BuildingType::CyberneticsCore: maxHealth=health=700; break;
            case BuildingType::Stargate: maxHealth=health=900; break;
            case BuildingType::FleetBeacon: maxHealth=health=600; break;
            case BuildingType::RoboticsBay: maxHealth=health=800; break;
            case BuildingType::Observatory: maxHealth=health=600; break;
        }
    }
    
    char glyph() const {
        if (!alive) return 'X';
        if (team==Team::Player) {
            switch(type) {
                case BuildingType::CommandCentre: return 'C';
                case BuildingType::Barracks: return 'B';
                case BuildingType::Factory: return 'F';
                case BuildingType::Starport: return 'S';
                case BuildingType::SupplyDepot: return 'D';
                case BuildingType::Refinery: return 'R';
                case BuildingType::Turret: return 'T';
                case BuildingType::Bunker: return 'U';
                case BuildingType::Academy: return 'A';
                case BuildingType::Armory: return 'M';
                case BuildingType::TechLab: return 'L';
                case BuildingType::ComSat: return 'O';
                case BuildingType::NuclearSilo: return 'N';
                case BuildingType::Pylon: return 'P';
                case BuildingType::Gateway: return 'G';
                case BuildingType::CyberneticsCore: return 'Y';
                case BuildingType::Stargate: return 'K';
                case BuildingType::FleetBeacon: return 'E';
                case BuildingType::RoboticsBay: return 'J';
                case BuildingType::Observatory: return 'V';
            }
        } else {
            switch(type) {
                case BuildingType::EnemyBase: return 'c';
                case BuildingType::EnemyBarracks: return 'b';
                case BuildingType::Turret: return 't';
                case BuildingType::Bunker: return 'u';
                default: return 'e';
            }
        }
        return '?';
    }
    
    int cost() const {
        switch(type) {
            case BuildingType::CommandCentre: return 400;
            case BuildingType::Barracks: return 150;
            case BuildingType::Factory: return 200;
            case BuildingType::Starport: return 250;
            case BuildingType::SupplyDepot: return 100;
            case BuildingType::Refinery: return 100;
            case BuildingType::Turret: return 75;
            case BuildingType::Bunker: return 100;
            case BuildingType::Academy: return 150;
            case BuildingType::Armory: return 200;
            case BuildingType::TechLab: return 150;
            case BuildingType::ComSat: return 150;
            case BuildingType::NuclearSilo: return 200;
            case BuildingType::Pylon: return 100;
            case BuildingType::Gateway: return 150;
            case BuildingType::CyberneticsCore: return 200;
            case BuildingType::Stargate: return 250;
            case BuildingType::FleetBeacon: return 200;
            case BuildingType::RoboticsBay: return 250;
            case BuildingType::Observatory: return 150;
            default: return 200;
        }
    }
    
    bool canTrain(UnitType ut) const {
        if (!alive || underConstruction) return false;
        switch(type) {
            case BuildingType::CommandCentre: return ut==UnitType::Worker;
            case BuildingType::Barracks: return ut==UnitType::Marine || ut==UnitType::Medic;
            case BuildingType::Factory: return ut==UnitType::Tank;
            case BuildingType::Starport: return ut==UnitType::Fighter;
            default: return false;
        }
    }
};

struct Bullet {
    Vec2 pos;
    Vec2 target;
    Team team;
    int damage;
    float speed;
    bool active;
    Bullet(Vec2 p, Vec2 t, Team tm, int d, float s):pos(p),target(t),team(tm),damage(d),speed(s),active(true) {}
};

struct Explosion {
    Vec2 pos;
    float radius;
    float age;
    float maxAge;
    Explosion(Vec2 p, float r, float a):pos(p),radius(r),age(0),maxAge(a) {}
};

struct TechTree {
    bool barracksBuilt = false;
    bool factoryBuilt = false;
    bool starportBuilt = false;
    bool stimPackResearched = false;
    bool siegeModeResearched = false;
};

struct MinimapData {
    std::vector<std::vector<char>> tiles;
    void generate(int w, int h) {
        tiles.resize(h, std::vector<char>(w, '.'));
    }
};

struct FogOfWar {
    std::vector<std::vector<bool>> visible;
    void generate(int w, int h) {
        visible.resize(h, std::vector<bool>(w, false));
    }
    void update(int camX, int camY, int viewW, int viewH, const std::vector<Unit>& units, const std::vector<Building>& buildings) {
        for (int y=0; y<h; y++) for (int x=0; x<w; x++) visible[y][x] = false;
        for (const auto& u: units) {
            if (u.team != Team::Player || !u.alive) continue;
            for (int dy=-VISION_RANGE; dy<=VISION_RANGE; dy++) {
                for (int dx=-VISION_RANGE; dx<=VISION_RANGE; dx++) {
                    if (dx*dx+dy*dy <= VISION_RANGE*VISION_RANGE) {
                        int tx = u.pos.x + dx;
                        int ty = u.pos.y + dy;
                        if (tx>=0 && tx<w && ty>=0 && ty<h) visible[ty][tx] = true;
                    }
                }
            }
        }
        for (const auto& b: buildings) {
            if (b.team != Team::Player || !b.alive) continue;
            for (int dy=-VISION_RANGE-2; dy<=VISION_RANGE+2; dy++) {
                for (int dx=-VISION_RANGE-2; dx<=VISION_RANGE+2; dx++) {
                    if (dx*dx+dy*dy <= (VISION_RANGE+2)*(VISION_RANGE+2)) {
                        int tx = b.pos.x + dx;
                        int ty = b.pos.y + dy;
                        if (tx>=0 && tx<w && ty>=0 && ty<h) visible[ty][tx] = true;
                    }
                }
            }
        }
    }
    int w, h;
};

struct GameStats {
    int unitsKilled = 0;
    int buildingsDestroyed = 0;
    int resourcesGathered = 0;
    float gameTime = 0;
    int wavesCompleted = 0;
};

Resource playerResources;
Resource enemyResources;
std::vector<Unit> units;
std::vector<Building> buildings;
std::vector<Bullet> bullets;
std::vector<Explosion> explosions;
std::vector<Vec2> mineralPatches;
std::vector<Vec2> gasPatches;
MinimapData minimap;
FogOfWar fog;
TechTree techTree;
GameStats stats;
int camX = 0, camY = 0;
int selectedCount = 0;
GameState gameState = GameState::Playing;
GameMode gameMode = GameMode::Skirmish;
int waveNumber = 0;
float waveTimer = 60.0f;
float gameTime = 0.0f;

void initGame() {
    srand(time(nullptr));
    
    // Player base
    buildings.emplace_back(Vec2(5,5), BuildingType::CommandCentre, Team::Player);
    buildings.emplace_back(Vec2(10,5), BuildingType::SupplyDepot, Team::Player);
    
    // Initial units
    units.emplace_back(Vec2(8,8), UnitType::Worker, Team::Player);
    units.emplace_back(Vec2(9,8), UnitType::Worker, Team::Player);
    units.emplace_back(Vec2(10,8), UnitType::Worker, Team::Player);
    
    // Enemy base
    buildings.emplace_back(Vec2(275,275), BuildingType::EnemyBase, Team::Enemy);
    buildings.emplace_back(Vec2(270,275), BuildingType::EnemyBarracks, Team::Enemy);
    
    // Initial enemy units
    units.emplace_back(Vec2(273,273), UnitType::Marine, Team::Enemy);
    units.emplace_back(Vec2(274,274), UnitType::Marine, Team::Enemy);
    units.emplace_back(Vec2(275,272), UnitType::Zergling, Team::Enemy);
    
    // Generate mineral patches
    for (int i=0; i<20; i++) {
        int x = rand() % (MAP_W - 40) + 20;
        int y = rand() % (MAP_H - 40) + 20;
        if (std::abs(x-5) > 20 || std::abs(y-5) > 20) {
            mineralPatches.emplace_back(x,y);
        }
    }
    
    // Generate gas patches near refineries
    for (int i=0; i<10; i++) {
        int x = rand() % (MAP_W - 40) + 20;
        int y = rand() % (MAP_H - 40) + 20;
        gasPatches.emplace_back(x,y);
    }
    
    minimap.generate(MAP_W, MAP_H);
    fog.w = MAP_W; fog.h = MAP_H;
    fog.generate(MAP_W, MAP_H);
    
    playerResources.minerals = 500;
    playerResources.supplyMax = 20;
    enemyResources.minerals = 500;
    enemyResources.supplyMax = 20;
}

void updateMinimap() {
    for (auto& row : minimap.tiles) std::fill(row.begin(), row.end(), '.');
    for (const auto& b : buildings) {
        if (b.pos.x >= 0 && b.pos.x < MAP_W && b.pos.y >= 0 && b.pos.y < MAP_H) {
            minimap.tiles[b.pos.y][b.pos.x] = b.glyph();
        }
    }
    for (const auto& u : units) {
        if (u.pos.x >= 0 && u.pos.x < MAP_W && u.pos.y >= 0 && u.pos.y < MAP_H) {
            if (u.alive) minimap.tiles[u.pos.y][u.pos.x] = u.glyph();
        }
    }
    for (const auto& m : mineralPatches) {
        if (m.x >= 0 && m.x < MAP_W && m.y >= 0 && m.y < MAP_H) {
            minimap.tiles[m.y][m.x] = '*';
        }
    }
}

bool isVisible(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return false;
    return fog.visible[y][x];
}

void gatherResources(Unit& u) {
    if (u.type != UnitType::Worker) return;
    
    // Find nearest mineral patch
    Vec2* nearest = nullptr;
    int nearestDist = 999;
    for (auto& m : mineralPatches) {
        int d = u.pos.dist(m);
        if (d < nearestDist) {
            nearestDist = d;
            nearest = &m;
        }
    }
    
    if (nearest && nearestDist > 1) {
        // Move towards minerals
        if (u.pos.x < nearest->x) u.pos.x++;
        else if (u.pos.x > nearest->x) u.pos.x--;
        if (u.pos.y < nearest->y) u.pos.y++;
        else if (u.pos.y > nearest->y) u.pos.y--;
    } else if (nearest && nearestDist <= 1) {
        // Gather
        playerResources.minerals += 2;
        stats.resourcesGathered += 2;
    }
}

void combatLogic(Unit& attacker) {
    if (!attacker.alive || attacker.attackCooldown > 0) return;
    if (attacker.type == UnitType::Worker) return; // Workers don't attack
    
    // Find nearest enemy
    Unit* target = nullptr;
    int nearestDist = 999;
    for (auto& u : units) {
        if (u.team == attacker.team || !u.alive) continue;
        int d = attacker.pos.dist(u.pos);
        if (d < nearestDist) {
            nearestDist = d;
            target = &u;
        }
    }
    
    // Check buildings too
    Building* bTarget = nullptr;
    for (auto& b : buildings) {
        if (b.team == attacker.team || !b.alive) continue;
        int d = attacker.pos.dist(b.pos);
        if (d < nearestDist) {
            nearestDist = d;
            bTarget = &b;
            target = nullptr;
        }
    }
    
    if (nearestDist <= ATTACK_RANGE_RANGED && (target || bTarget)) {
        if (target) {
            bullets.emplace_back(attacker.pos, target->pos, attacker.team, attacker.attackPower - target->armor, 10.0f);
        } else if (bTarget) {
            bullets.emplace_back(attacker.pos, bTarget->pos, attacker.team, attacker.attackPower, 10.0f);
        }
        attacker.attackCooldown = 1.0f;
    }
}

void combatLogicEnemy(Unit& attacker) {
    if (!attacker.alive || attacker.attackCooldown > 0) return;
    
    // Find nearest player unit
    Unit* target = nullptr;
    int nearestDist = 999;
    for (auto& u : units) {
        if (u.team != Team::Enemy || !u.alive) continue;
        int d = attacker.pos.dist(u.pos);
        if (d < nearestDist) {
            nearestDist = d;
            target = &u;
        }
    }
    
    Building* bTarget = nullptr;
    for (auto& b : buildings) {
        if (b.team != Team::Enemy || !b.alive) continue;
        int d = attacker.pos.dist(b.pos);
        if (d < nearestDist) {
            nearestDist = d;
            bTarget = &b;
        }
    }
    
    if (nearestDist <= ATTACK_RANGE_RANGED && (target || bTarget)) {
        if (target) {
            bullets.emplace_back(attacker.pos, target->pos, attacker.team, attacker.attackPower - target->armor, 10.0f);
        } else if (bTarget) {
            bullets.emplace_back(attacker.pos, bTarget->pos, attacker.team, attacker.attackPower, 10.0f);
        }
        attacker.attackCooldown = 1.0f;
    }
}

void moveUnit(Unit& u, Vec2 target) {
    if (u.pos.dist(target) <= 1) return;
    if (u.pos.x < target.x) u.pos.x++;
    else if (u.pos.x > target.x) u.pos.x--;
    if (u.pos.y < target.y) u.pos.y++;
    else if (u.pos.y > target.y) u.pos.y--;
}

void attackMove(Unit& u, Vec2 target) {
    moveUnit(u, target);
    combatLogic(u);
}

void spawnWave() {
    waveNumber++;
    int unitCount = 3 + waveNumber * 2;
    
    for (int i=0; i<unitCount; i++) {
        int x = 275 + rand() % 5;
        int y = 275 + rand() % 5;
        UnitType type;
        if (waveNumber < 3) {
            type = (rand() % 2) ? UnitType::Marine : UnitType::Zergling;
        } else if (waveNumber < 5) {
            type = (rand() % 3) ? UnitType::Marine : (rand() % 2 ? UnitType::Zergling : UnitType::Tank);
        } else {
            int r = rand() % 5;
            if (r == 0) type = UnitType::Zergling;
            else if (r == 1) type = UnitType::Ultralisk;
            else type = UnitType::Tank;
        }
        units.emplace_back(Vec2(x, y), type, Team::Enemy);
    }
    
    waveTimer = 45.0f; // Next wave in 45 seconds
}

void update(float dt) {
    if (gameState != GameState::Playing) return;
    
    gameTime += dt;
    stats.gameTime = gameTime;
    
    // Update cooldowns
    for (auto& u : units) {
        if (u.attackCooldown > 0) u.attackCooldown -= dt;
    }
    
    // Resource gathering
    for (auto& u : units) {
        if (u.team == Team::Player && u.alive && u.type == UnitType::Worker) {
            gatherResources(u);
        }
    }
    
    // Player unit AI
    for (auto& u : units) {
        if (u.team != Team::Player || !u.alive) continue;
        
        // If has target position and not there yet, move
        if (u.pos.dist(u.targetPos) > 1 && u.type != UnitType::Worker) {
            moveUnit(u, u.targetPos);
        }
        
        // Combat
        combatLogic(u);
        
        // Medic healing
        if (u.type == UnitType::Medic) {
            for (auto& ally : units) {
                if (ally.team == Team::Player && ally.alive && ally.health < ally.maxHealth && u.pos.dist(ally.pos) <= 3) {
                    ally.health = std::min<int>(ally.maxHealth, ally.health + 5 * dt);
                }
            }
        }
    }
    
    // Enemy AI
    for (auto& u : units) {
        if (u.team != Team::Enemy || !u.alive) continue;
        
        // Find target (player base or units)
        Vec2 targetPos(5, 5);
        for (const auto& b : buildings) {
            if (b.team == Team::Player && b.alive) {
                targetPos = b.pos;
                break;
            }
        }
        
        // Move towards target
        if (u.pos.dist(targetPos) > 5) {
            moveUnit(u, targetPos);
        }
        
        combatLogicEnemy(u);
    }
    
    // Update bullets
    for (auto& b : bullets) {
        if (!b.active) continue;
        
        Vec2 dir = b.target - b.pos;
        int dist = std::abs(dir.x) + std::abs(dir.y);
        
        if (dist <= 1) {
            // Hit target
            for (auto& u : units) {
                if (u.pos == b.target && u.team != b.team && u.alive) {
                    u.health -= b.damage;
                    if (u.health <= 0) {
                        u.alive = false;
                        if (b.team == Team::Player) stats.unitsKilled++;
                    }
                }
            }
            for (auto& bld : buildings) {
                if (bld.pos == b.target && bld.team != b.team && bld.alive) {
                    bld.health -= b.damage;
                    if (bld.health <= 0) {
                        bld.alive = false;
                        if (b.team == Team::Player) stats.buildingsDestroyed++;
                    }
                }
            }
            explosions.emplace_back(b.pos, 0.5f, 0.3f);
            b.active = false;
        } else {
            // Move bullet
            if (dir.x > 0) b.pos.x++;
            else if (dir.x < 0) b.pos.x--;
            if (dir.y > 0) b.pos.y++;
            else if (dir.y < 0) b.pos.y--;
        }
    }
    
    // Update explosions
    for (auto& e : explosions) {
        e.age += dt;
    }
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& e){ return e.age >= e.maxAge; }), explosions.end());
    
    // Update bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b){ return !b.active; }), bullets.end());
    
    // Remove dead units
    units.erase(std::remove_if(units.begin(), units.end(), [](const Unit& u){ return !u.alive; }), units.end());
    
    // Check win/lose conditions
    bool playerBaseAlive = false;
    bool enemyBaseAlive = false;
    for (const auto& b : buildings) {
        if (b.team == Team::Player && b.alive && b.type == BuildingType::CommandCentre) playerBaseAlive = true;
        if (b.team == Team::Enemy && b.alive) enemyBaseAlive = true;
    }
    
    if (!playerBaseAlive) {
        gameState = GameState::GameOver;
    } else if (!enemyBaseAlive) {
        gameState = GameState::Victory;
    }
    
    // Wave spawning
    waveTimer -= dt;
    if (waveTimer <= 0 && enemyBaseAlive) {
        spawnWave();
    }
    
    // Update fog of war
    fog.update(camX, camY, COLS, LINES-3, units, buildings);
    
    // Update minimap
    updateMinimap();
    
    // Count selected
    selectedCount = 0;
    for (const auto& u : units) if (u.selected && u.team == Team::Player) selectedCount++;
    for (const auto& b : buildings) if (b.selected && b.team == Team::Player) selectedCount++;
}

void draw() {
    clear();
    
    // Draw visible map
    for (int sy=0; sy<LINES-3; sy++) {
        for (int sx=0; sx<COLS; sx++) {
            int wx = sx + camX;
            int wy = sy + camY;
            if (wx < 0 || wx >= MAP_W || wy < 0 || wy >= MAP_H) continue;
            
            if (!isVisible(wx, wy)) {
                mvaddch(sy, sx, ' ');
                continue;
            }
            
            // Terrain
            bool drawn = false;
            
            // Minerals
            for (const auto& m : mineralPatches) {
                if (m.x == wx && m.y == wy) {
                    attron(COLOR_PAIR(1));
                    mvaddch(sy, sx, '*');
                    attroff(COLOR_PAIR(1));
                    drawn = true;
                    break;
                }
            }
            
            if (!drawn) {
                mvaddch(sy, sx, '.');
            }
        }
    }
    
    // Draw buildings
    for (const auto& b : buildings) {
        if (!b.alive || !isVisible(b.pos.x, b.pos.y)) continue;
        int sx = b.pos.x - camX;
        int sy = b.pos.y - camY;
        if (sx >= 0 && sx < COLS && sy >= 0 && sy < LINES-3) {
            attron(b.selected ? A_REVERSE : A_NORMAL);
            if (b.team == Team::Enemy) attron(COLOR_PAIR(2));
            else if (b.underConstruction) attron(COLOR_PAIR(3));
            mvaddch(sy, sx, b.glyph());
            if (b.team == Team::Enemy) attroff(COLOR_PAIR(2));
            else if (b.underConstruction) attroff(COLOR_PAIR(3));
            attroff(A_REVERSE);
            
            // Health bar
            if (b.health < b.maxHealth) {
                double pct = (double)b.health / b.maxHealth;
                mvprintw(sy, sx-1, "[%s]", pct > 0.5 ? "###" : pct > 0.25 ? "#.." : "...");
            }
        }
    }
    
    // Draw units
    for (const auto& u : units) {
        if (!u.alive || !isVisible(u.pos.x, u.pos.y)) continue;
        int sx = u.pos.x - camX;
        int sy = u.pos.y - camY;
        if (sx >= 0 && sx < COLS && sy >= 0 && sy < LINES-3) {
            attron(u.selected ? A_REVERSE : A_NORMAL);
            if (u.team == Team::Enemy) attron(COLOR_PAIR(2));
            mvaddch(sy, sx, u.glyph());
            if (u.team == Team::Enemy) attroff(COLOR_PAIR(2));
            attroff(A_REVERSE);
            
            // Health bar for damaged units
            if (u.health < u.maxHealth) {
                double pct = (double)u.health / u.maxHealth;
                mvprintw(sy-1, sx-1, "[%d]", u.health);
            }
        }
    }
    
    // Draw explosions
    for (const auto& e : explosions) {
        int sx = e.pos.x - camX;
        int sy = e.pos.y - camY;
        if (sx >= 0 && sx < COLS && sy >= 0 && sy < LINES-3) {
            attron(COLOR_PAIR(4));
            mvaddch(sy, sx, '*');
            attroff(COLOR_PAIR(4));
        }
    }
    
    // Draw bullets
    for (const auto& b : bullets) {
        int sx = b.pos.x - camX;
        int sy = b.pos.y - camY;
        if (sx >= 0 && sx < COLS && sy >= 0 && sy < LINES-3) {
            attron(COLOR_PAIR(5));
            mvaddch(sy, sx, '.');
            attroff(COLOR_PAIR(5));
        }
    }
    
    // HUD
    int y = LINES - 3;
    attron(A_BOLD);
    mvprintw(y, 0, " Minerals: %d | Gas: %d | Supply: %d/%d | Wave: %d (%.0fs) | Time: %.0fs ",
        playerResources.minerals, playerResources.gas,
        playerResources.supplyUsed, playerResources.supplyMax,
        waveNumber, waveTimer, gameTime);
    attroff(A_BOLD);
    
    y++;
    std::string controls = "s:worker t:marine b:barracks c:centre m:move a:attack WASD/arrows:pan q:quit";
    mvprintw(y, 0, "%-*s", COLS, controls.c_str());
    
    y++;
    mvprintw(y, 0, "Selected: %d | Kills: %d | Destroyed: %d | Gathered: %d",
        selectedCount, stats.unitsKilled, stats.buildingsDestroyed, stats.resourcesGathered);
    
    // Game state overlay
    if (gameState == GameState::GameOver) {
        attron(A_BOLD | COLOR_PAIR(2));
        mvprintw(LINES/2, COLS/2 - 10, "GAME OVER");
        mvprintw(LINES/2 + 1, COLS/2 - 15, "Press R to restart");
        attroff(A_BOLD | COLOR_PAIR(2));
    } else if (gameState == GameState::Victory) {
        attron(A_BOLD | COLOR_PAIR(1));
        mvprintw(LINES/2, COLS/2 - 10, "VICTORY!");
        mvprintw(LINES/2 + 1, COLS/2 - 15, "Press R to restart");
        attroff(A_BOLD | COLOR_PAIR(1));
    }
    
    refresh();
}

bool isOccupied(Vec2 pos) {
    for (const auto& u : units) {
        if (u.alive && u.pos.dist(pos) < 2) return true;
    }
    for (const auto& b : buildings) {
        if (b.alive && b.pos.dist(pos) < 3) return true;
    }
    return false;
}

Building* findNearestBuilding(Vec2 pos, BuildingType type, Team team) {
    Building* nearest = nullptr;
    int nearestDist = 9999;
    for (auto& b : buildings) {
        if (b.type == type && b.team == team && b.alive) {
            int d = b.pos.dist(pos);
            if (d < nearestDist) {
                nearestDist = d;
                nearest = &b;
            }
        }
    }
    return nearest;
}

void handleInput() {
    int ch = getch();
    
    if (gameState == GameState::GameOver || gameState == GameState::Victory) {
        if (ch == 'r' || ch == 'R') {
            units.clear();
            buildings.clear();
            bullets.clear();
            explosions.clear();
            mineralPatches.clear();
            gasPatches.clear();
            techTree = TechTree();
            stats = GameStats();
            waveNumber = 0;
            waveTimer = 60.0f;
            gameTime = 0.0f;
            gameState = GameState::Playing;
            initGame();
        }
        return;
    }
    
    switch(ch) {
        case 'q': case 'Q':
            endwin();
            exit(0);
        case KEY_UP: case 'w': case 'W':
            camY = std::max(0, camY - 3);
            break;
        case KEY_DOWN: case 's': case 'S':
            camY = std::min(MAP_H - LINES + 3, camY + 3);
            break;
        case KEY_LEFT: case 'a': case 'A':
            camX = std::max(0, camX - 3);
            break;
        case KEY_RIGHT: case 'd': case 'D':
            camX = std::min(MAP_W - COLS, camX + 3);
            break;
        case 'p': case 'P':
            gameState = (gameState == GameState::Paused) ? GameState::Playing : GameState::Paused;
            break;
        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
            int idx = ch - '1';
            int count = 0;
            for (auto& u : units) {
                if (u.team == Team::Player && u.alive && count == idx) {
                    u.selected = !u.selected;
                    break;
                }
                if (u.team == Team::Player && u.alive) count++;
            }
            break;
        }
        case KEY_MOUSE: {
            MEVENT ev;
            if (getmouse(&ev) == OK) {
                int wx = ev.x + camX;
                int wy = ev.y + camY;
                
                if (ev.bstate & BUTTON1_CLICKED) {
                    // Check if clicked on unit/building
                    bool clickedUnit = false;
                    for (auto& u : units) {
                        if (u.pos.x == wx && u.pos.y == wy && u.team == Team::Player && u.alive) {
                            u.selected = !u.selected;
                            clickedUnit = true;
                            break;
                        }
                    }
                    if (!clickedUnit) {
                        // Deselect all
                        for (auto& u : units) u.selected = false;
                        for (auto& b : buildings) b.selected = false;
                    }
                } else if (ev.bstate & BUTTON3_CLICKED) {
                    // Right click - command
                    for (auto& u : units) {
                        if (u.selected && u.team == Team::Player) {
                            u.targetPos = Vec2(wx, wy);
                        }
                    }
                }
            }
            break;
        }
        case ' ': {
            // Build mode
            break;
        }
        default:
            break;
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, nullptr);
    nodelay(stdscr, TRUE);
    
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Minerals
        init_pair(2, COLOR_RED, COLOR_BLACK);    // Enemy
        init_pair(3, COLOR_CYAN, COLOR_BLACK);   // Building
        init_pair(4, COLOR_MAGENTA, COLOR_BLACK);// Explosion
        init_pair(5, COLOR_GREEN, COLOR_BLACK);  // Bullet
    }
    
    initGame();
    
    float lastTime = clock();
    while (true) {
        float currentTime = clock();
        float dt = (currentTime - lastTime) / CLOCKS_PER_SEC;
        lastTime = currentTime;
        
        handleInput();
        if (gameState == GameState::Playing) {
            update(dt);
        }
        draw();
        napms(16); // ~60 FPS
    }
    
    endwin();
    return 0;
}
