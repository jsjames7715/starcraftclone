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
#include <map>
#include <functional>
#include <sstream>

struct Vec2 { int x, y; Vec2(int X=0,int Y=0):x(X),y(Y){} Vec2 operator+(const Vec2& o)const{return Vec2(x+o.x,y+o.y);} Vec2 operator-(const Vec2& o)const{return Vec2(x-o.x,y-o.y);} int dist(const Vec2& o)const{return std::abs(x-o.x)+std::abs(y-o.y);} bool operator==(const Vec2& o)const{return x==o.x&&y==o.y;} bool operator!=(const Vec2& o)const{return x!=o.x||y!=o.y;} };
struct Vec3 { float x,y,z; Vec3(float X=0,float Y=0,float Z=0):x(X),y(Y),z(Z){} Vec3 normalized()const{float l=std::sqrt(x*x+y*y+z*z);return l?Vec3(x/l,y/l,z/l):Vec3();} };

enum class Team { Player, Enemy, Neutral };

enum class UnitType { 
    Worker, Marine, Medic, Tank, Fighter, Zergling, Ultralisk,
    Ghost, Vulture, Goliath, Wraith, Battlecruiser, ScienceVessel,
    Firebat, Dropship, Valkyrie, Corsair, Scout, DarkTemplar,
    Dragoon, Zealot, Archon, Carrier, Reaver, Shuttle,
    Mutalisk, Hydralisk, Defiler, Lurker, Broodling, Guardian,
    Devourer, SporeColony, SunkenColony, Lair, Hive, UltraliskDen,
    ScienceFacility, CovertOps, PhysicsLab, FusionCore, MachineShop,
    EngineeringBay, WeaponRefinery, CommandCenter, Nexus, Probe,
    Arbiter, HighTemplar, Interceptor, Scarab, Overlord, Creeper,
    Queen, SpawningPool, EvolutionChamber, Extractor, Spire, NydusCanal,
    Baneling, Roach, Infestor, SwarmHost, Viper, Overseer
};

enum class BuildingType {
    CommandCentre, Barracks, Factory, Starport, SupplyDepot, Refinery,
    EnemyBase, EnemyBarracks, Turret, Bunker, Academy, Armory, TechLab,
    ComSat, NuclearSilo, SpiderMine, Pylon, Gateway, CyberneticsCore,
    Stargate, FleetBeacon, RoboticsBay, Observatory,
    Hatchery, Lair, Hive, Spire, GreaterSpire, SpawningPool, EvolutionChamber,
    Extractor, HydraliskDen, LurkerAspect, SporeColony, SunkenColony,
    UltraliskCavern, NydusCanal, InfestationPit, GreatHall,
    Nexus, GatewayZerg, CyberneticsZerg, StargateProtoss, FleetBeaconProtoss,
    RoboticsBayProtoss, TwilightCouncil, DarkShrine, TemplarArchives,
    Forge, RoboticsFacility, ObservatoryProtoss, Cannon, PhotonCannon,
    ShieldBattery, WarpGate, assimilator, PsiDisruptor, UltraliskDenZerg,
    CreepTumor, InfestorTerran, GhostAcademy, MissileTurret, PointDefense,
    PlanetaryFortress, OrbitalCommand, NuclearMissile, TechLabZerg, WarpPrism, 
    Disruptor, Oracle, Tempest, Mothership, MothershipCore, NexusProbe, 
    PhotonCannonBuilding, ShieldBatteryBuilding, EngineeringBay
};

enum class GameState { Playing, Paused, GameOver, Victory, Menu };
enum class GameMode { Campaign, Skirmish, Deathmatch, Survival, Rush, Economic, TowerDefense, CaptureTheFlag, KingOfTheHill, Custom, Arena, Blitz, Puzzle, BossRush, Escort, Assault, Defense, Recon, Sabotage, HeroMode, Nightmare, IronMan, WeeklyChallenge, DailyChallenge, Tournament, Multiplayer, LAN, Online, Cooperative, Solo, Practice, Tutorial, Challenge, Endless, gauntlet };

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
            case UnitType::Mutalisk: maxHealth=health=120; attackPower=25; armor=1; break;
            case UnitType::Hydralisk: maxHealth=health=90; attackPower=15; armor=0; break;
            case UnitType::Defiler: maxHealth=health=80; attackPower=0; armor=0; break;
            case UnitType::Lurker: maxHealth=health=120; attackPower=30; armor=1; break;
            case UnitType::Broodling: maxHealth=health=30; attackPower=8; armor=0; break;
            case UnitType::Guardian: maxHealth=health=150; attackPower=40; armor=2; break;
            case UnitType::Devourer: maxHealth=health=250; attackPower=35; armor=3; break;
            case UnitType::Queen: maxHealth=health=100; attackPower=15; armor=1; break;
            case UnitType::Baneling: maxHealth=health=30; attackPower=40; armor=0; break;
            case UnitType::Roach: maxHealth=health=100; attackPower=18; armor=1; break;
            case UnitType::Infestor: maxHealth=health=90; attackPower=0; armor=0; break;
            case UnitType::SwarmHost: maxHealth=health=140; attackPower=0; armor=2; break;
            case UnitType::Viper: maxHealth=health=120; attackPower=0; armor=1; break;
            case UnitType::Overseer: maxHealth=health=200; attackPower=0; armor=2; break;
            case UnitType::Arbiter: maxHealth=health=300; attackPower=20; armor=3; break;
            case UnitType::HighTemplar: maxHealth=health=80; attackPower=0; armor=0; break;
            case UnitType::Interceptor: maxHealth=health=40; attackPower=8; armor=0; break;
            case UnitType::Scarab: maxHealth=health=50; attackPower=20; armor=0; break;
            case UnitType::Overlord: maxHealth=health=400; attackPower=0; armor=2; break;
            case UnitType::Probe: maxHealth=health=50; attackPower=5; armor=0; break;
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
                case UnitType::Mutalisk: return 'm';
                case UnitType::Hydralisk: return 'h';
                case UnitType::Defiler: return 'd';
                case UnitType::Lurker: return 'l';
                case UnitType::Broodling: return 'b';
                case UnitType::Guardian: return 'g';
                case UnitType::Devourer: return 'v';
                case UnitType::Queen: return 'q';
                case UnitType::Baneling: return 'n';
                case UnitType::Roach: return 'r';
                case UnitType::Infestor: return 'i';
                case UnitType::SwarmHost: return 's';
                case UnitType::Viper: return 'x';
                case UnitType::Overseer: return 'o';
                case UnitType::Arbiter: return 'a';
                case UnitType::HighTemplar: return 't';
                case UnitType::Interceptor: return 'j';
                case UnitType::Scarab: return 'c';
                case UnitType::Overlord: return 'u';
                case UnitType::Probe: return 'p';
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
            case UnitType::Mutalisk: return 100;
            case UnitType::Hydralisk: return 75;
            case UnitType::Defiler: return 100;
            case UnitType::Lurker: return 150;
            case UnitType::Broodling: return 30;
            case UnitType::Guardian: return 200;
            case UnitType::Devourer: return 250;
            case UnitType::Queen: return 125;
            case UnitType::Baneling: return 50;
            case UnitType::Roach: return 75;
            case UnitType::Infestor: return 150;
            case UnitType::SwarmHost: return 200;
            case UnitType::Viper: return 250;
            case UnitType::Overseer: return 150;
            case UnitType::Arbiter: return 300;
            case UnitType::HighTemplar: return 150;
            case UnitType::Interceptor: return 50;
            case UnitType::Scarab: return 75;
            case UnitType::Overlord: return 100;
            case UnitType::Probe: return 50;
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
            case BuildingType::Hatchery: maxHealth=health=1200; break;
            case BuildingType::Lair: maxHealth=health=1500; break;
            case BuildingType::Hive: maxHealth=health=1800; break;
            case BuildingType::Spire: maxHealth=health=800; break;
            case BuildingType::GreaterSpire: maxHealth=health=1000; break;
            case BuildingType::SpawningPool: maxHealth=health=600; break;
            case BuildingType::EvolutionChamber: maxHealth=health=500; break;
            case BuildingType::Extractor: maxHealth=health=400; break;
            case BuildingType::HydraliskDen: maxHealth=health=700; break;
            case BuildingType::SporeColony: maxHealth=health=400; size=Vec2(1,1); break;
            case BuildingType::SunkenColony: maxHealth=health=400; size=Vec2(1,1); break;
            case BuildingType::UltraliskCavern: maxHealth=health=800; break;
            case BuildingType::NydusCanal: maxHealth=health=600; break;
            case BuildingType::InfestationPit: maxHealth=health=700; break;
            case BuildingType::GreatHall: maxHealth=health=1400; break;
            case BuildingType::Nexus: maxHealth=health=1000; break;
            case BuildingType::Forge: maxHealth=health=600; break;
            case BuildingType::RoboticsFacility: maxHealth=health=800; break;
            case BuildingType::TwilightCouncil: maxHealth=health=700; break;
            case BuildingType::DarkShrine: maxHealth=health=500; break;
            case BuildingType::TemplarArchives: maxHealth=health=600; break;
            case BuildingType::PhotonCannon: maxHealth=health=400; size=Vec2(1,1); break;
            case BuildingType::ShieldBattery: maxHealth=health=300; size=Vec2(1,1); break;
            case BuildingType::WarpGate: maxHealth=health=600; break;
            case BuildingType::PlanetaryFortress: maxHealth=health=1500; break;
            case BuildingType::OrbitalCommand: maxHealth=health=1200; break;
            case BuildingType::EngineeringBay: maxHealth=health=600; break;
            case BuildingType::GhostAcademy: maxHealth=health=700; break;
            case BuildingType::MissileTurret: maxHealth=health=400; size=Vec2(1,1); break;
            case BuildingType::PointDefense: maxHealth=health=350; size=Vec2(1,1); break;
            case BuildingType::WarpPrism: maxHealth=health=600; break;
            case BuildingType::Disruptor: maxHealth=health=700; break;
            case BuildingType::Oracle: maxHealth=health=500; break;
            case BuildingType::Tempest: maxHealth=health=800; break;
            case BuildingType::Mothership: maxHealth=health=1500; break;
            case BuildingType::MothershipCore: maxHealth=health=1000; break;
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
                case BuildingType::Hatchery: return 'H';
                case BuildingType::Lair: return 'L';
                case BuildingType::Hive: return 'I';
                case BuildingType::Spire: return 'X';
                case BuildingType::SpawningPool: return 'S';
                case BuildingType::EvolutionChamber: return 'E';
                case BuildingType::Extractor: return 'X';
                case BuildingType::HydraliskDen: return 'H';
                case BuildingType::SporeColony: return 'S';
                case BuildingType::SunkenColony: return 'U';
                case BuildingType::UltraliskCavern: return 'U';
                case BuildingType::NydusCanal: return 'N';
                case BuildingType::InfestationPit: return 'I';
                case BuildingType::GreatHall: return 'G';
                case BuildingType::Nexus: return 'N';
                case BuildingType::Forge: return 'F';
                case BuildingType::RoboticsFacility: return 'R';
                case BuildingType::TwilightCouncil: return 'T';
                case BuildingType::DarkShrine: return 'D';
                case BuildingType::TemplarArchives: return 'T';
                case BuildingType::PhotonCannon: return 'P';
                case BuildingType::ShieldBattery: return 'B';
                case BuildingType::WarpGate: return 'W';
                case BuildingType::PlanetaryFortress: return 'F';
                case BuildingType::OrbitalCommand: return 'O';
                case BuildingType::EngineeringBay: return 'E';
                case BuildingType::GhostAcademy: return 'G';
                case BuildingType::MissileTurret: return 'M';
                case BuildingType::PointDefense: return 'P';
                case BuildingType::WarpPrism: return 'W';
                case BuildingType::Disruptor: return 'D';
                case BuildingType::Oracle: return 'O';
                case BuildingType::Tempest: return 'T';
                case BuildingType::Mothership: return 'M';
                case BuildingType::MothershipCore: return 'C';
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
    int totalDamageDealt = 0;
    int totalDamageTaken = 0;
    int unitsBuilt = 0;
    int buildingsBuilt = 0;
    int missionsCompleted = 0;
    int bossesDefeated = 0;
    int fastestWave = 999;
    int highestWave = 0;
    int totalKills = 0;
    int totalDeaths = 0;
    int resourcesSpent = 0;
    int resourcesEarned = 0;
};

struct Achievement {
    std::string name;
    std::string description;
    bool unlocked = false;
    int requiredValue;
    int currentValue = 0;
};
std::vector<Achievement> achievements = {
    {"First Blood", "Kill your first enemy unit", false, 1},
    {"Rusher", "Complete wave 1", false, 1},
    {"Veteran", "Complete wave 5", false, 5},
    {"Elite", "Complete wave 10", false, 10},
    {"Legendary", "Complete wave 20", false, 20},
    {"Economist", "Gather 1000 minerals", false, 1000},
    {"Wealthy", "Gather 5000 minerals", false, 5000},
    {"Builder", "Build 10 buildings", false, 10},
    {"Constructor", "Build 25 buildings", false, 25},
    {"Army Builder", "Train 50 units", false, 50},
    {"Horde", "Train 100 units", false, 100},
    {"Destroyer", "Destroy 10 enemy buildings", false, 10},
    {"Raider", "Destroy 25 enemy buildings", false, 25},
    {"Overwhelming Force", "Destroy 50 enemy buildings", false, 50},
    {"Speed Demon", "Complete wave 3 in under 30 seconds", false, 1},
    {"Survivalist", "Survive 10 waves without losing a building", false, 1},
    {"Clean Sweep", "Destroy all enemy buildings in under 5 minutes", false, 1},
    {"Efficiency Expert", "Gather 2000 minerals with only 5 workers", false, 1},
    {"Turtle", "Win a game with 20 turrets", false, 1},
    {"Blitz", "Win a game in under 3 minutes", false, 1},
    {"Marathon", "Play for 30 minutes", false, 1},
    {"Boss Slayer", "Defeat a boss enemy", false, 1},
    {"Boss Destroyer", "Defeat 5 boss enemies", false, 5},
    {"Collector", "Collect 10 power-ups", false, 10},
    {"Power Overwhelming", "Collect 25 power-ups", false, 25},
    {"Upgrade Master", "Research 5 upgrades", false, 5}
};

struct PowerUp {
    Vec2 pos;
    std::string type;
    float lifetime;
    bool active = true;
    PowerUp(Vec2 p, std::string t, float l):pos(p),type(t),lifetime(l){}
};
std::vector<PowerUp> powerups;

struct Boss {
    std::string name;
    int health;
    int maxHealth;
    int damage;
    int attackRange;
    float attackSpeed;
    Vec2 pos;
    bool alive;
    std::string type;
    Boss(std::string n, int h, int d, int ar, float as, Vec2 p, std::string t):
        name(n), health(h), maxHealth(h), damage(d), attackRange(ar), attackSpeed(as), pos(p), alive(true), type(t) {}
};
std::vector<Boss> bosses;

struct Weather {
    std::string currentWeather;
    float intensity;
    float duration;
    float timer;
    bool active;
};
Weather weather = {"Clear", 0.0f, 0.0f, 0.0f, false};

struct DayNightCycle {
    float timeOfDay;
    float cycleDuration;
    bool isDay;
    float lightLevel;
};
DayNightCycle dayNight = {0.5f, 120.0f, true, 1.0f};

struct Upgrade {
    std::string name;
    std::string description;
    int mineralCost;
    int gasCost;
    int researchTime;
    bool researched = false;
    std::string effect;
};
std::vector<Upgrade> upgrades = {
    {"Stim Pack", "Increases unit attack speed by 20%", 100, 100, 60, false, "attackSpeed"},
    {"Shield Battery", "Regenerates shields faster", 150, 100, 90, false, "shieldRegen"},
    {"Marine Range", "Increases marine attack range by 2", 100, 75, 45, false, "range"},
    {"Marine Damage", "Increases marine damage by 5", 100, 75, 45, false, "damage"},
    {"Tank Siege Mode", "Unlocks siege mode for tanks", 150, 100, 90, false, "siege"},
    {"Medic Healing", "Increases medic healing by 50%", 100, 100, 60, false, "healing"},
    {"Ghost Cloak", "Allows ghost to cloak", 200, 150, 120, false, "cloak"},
    {"Vulture Speed", "Increases vulture movement speed", 100, 75, 45, false, "speed"},
    {"Goliath Range", "Increases goliath attack range", 125, 100, 60, false, "range"},
    {"Wraith Cloak", "Allows wraith to cloak", 200, 150, 120, false, "cloak"},
    {"Battlecruiser Yamato", "Unlocks yamato cannon", 250, 200, 150, false, "yamato"},
    {"Science Vessel Defensive", "Increases science vessel durability", 150, 150, 90, false, "durability"},
    {"Firebat Damage", "Increases firebat damage by 10", 100, 75, 45, false, "damage"},
    {"Dropship Capacity", "Increases dropship unit capacity", 100, 75, 45, false, "capacity"},
    {"Valkyrie Damage", "Increases valkyrie damage against air", 175, 125, 90, false, "airDamage"}
};

struct Leaderboard {
    std::string playerName;
    int score;
    int wavesCompleted;
    float gameTime;
    int kills;
};
std::vector<Leaderboard> leaderboard;

struct Particle {
    Vec2 pos;
    Vec2 vel;
    float lifetime;
    float age;
    char glyph;
    bool active;
    Particle(Vec2 p, Vec2 v, float l, char g):pos(p),vel(v),lifetime(l),age(0),glyph(g),active(true){}
};
std::vector<Particle> particles;

enum class TerrainType { Grass, Water, Mountain, Forest, Sand, Rock, Lava, Ice, Swamp, Desert, Snow, Cliff, Canyon, Valley, Beach };
enum class SpecialAbility { None, Stim, Heal, Cloak, SiegeMode, Yamato, Irradiate, PsionicStorm, Lockdown,EMP, ScannerSweep, DefensiveMatrix, Restoration, OpticalFlare, EMPShockwave, SpawnBroodlings, DarkSwarm, Plague, Consume, Burrow, SpawnLurker, ArchonWarp, PhaseShift, StasisField, PsionicHold, Feedback, Maelstrom, Parasite, Ensnare, GravityFlux, Vortex };

struct Terrain {
    std::vector<std::vector<TerrainType>> tiles;
    std::vector<std::vector<int>> elevation;
    void generate(int w, int h) {
        tiles.resize(h, std::vector<TerrainType>(w, TerrainType::Grass));
        elevation.resize(h, std::vector<int>(w, 0));
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int r = rand() % 100;
                if (r < 5) tiles[y][x] = TerrainType::Water;
                else if (r < 10) tiles[y][x] = TerrainType::Mountain;
                else if (r < 15) tiles[y][x] = TerrainType::Forest;
                else if (r < 18) tiles[y][x] = TerrainType::Sand;
                else if (r < 20) tiles[y][x] = TerrainType::Rock;
                else if (r < 22) tiles[y][x] = TerrainType::Lava;
                else if (r < 24) tiles[y][x] = TerrainType::Ice;
                else if (r < 26) tiles[y][x] = TerrainType::Swamp;
                else if (r < 28) tiles[y][x] = TerrainType::Desert;
                else if (r < 30) tiles[y][x] = TerrainType::Snow;
                elevation[y][x] = rand() % 5;
            }
        }
    }
    char getGlyph(int x, int y) const {
        if (x < 0 || x >= (int)tiles[0].size() || y < 0 || y >= (int)tiles.size()) return '.';
        switch(tiles[y][x]) {
            case TerrainType::Grass: return '.';
            case TerrainType::Water: return '~';
            case TerrainType::Mountain: return '^';
            case TerrainType::Forest: return '%';
            case TerrainType::Sand: return '_';
            case TerrainType::Rock: return 'O';
            case TerrainType::Lava: return '*';
            case TerrainType::Ice: return '#';
            case TerrainType::Swamp: return '~';
            case TerrainType::Desert: return '.';
            case TerrainType::Snow: return '*';
            case TerrainType::Cliff: return '|';
            case TerrainType::Canyon: return 'V';
            case TerrainType::Valley: return 'U';
            case TerrainType::Beach: return '=';
        }
        return '.';
    }
    bool isWalkable(int x, int y) const {
        if (x < 0 || x >= (int)tiles[0].size() || y < 0 || y >= (int)tiles.size()) return false;
        TerrainType t = tiles[y][x];
        return t != TerrainType::Water && t != TerrainType::Mountain && t != TerrainType::Lava && t != TerrainType::Cliff && t != TerrainType::Canyon;
    }
    float getMoveCost(int x, int y) const {
        if (!isWalkable(x, y)) return 999.0f;
        if (x < 0 || x >= (int)tiles[0].size() || y < 0 || y >= (int)tiles.size()) return 999.0f;
        switch(tiles[y][x]) {
            case TerrainType::Grass: return 1.0f;
            case TerrainType::Forest: return 1.5f;
            case TerrainType::Sand: return 1.2f;
            case TerrainType::Rock: return 1.3f;
            case TerrainType::Ice: return 1.1f;
            case TerrainType::Swamp: return 2.0f;
            case TerrainType::Desert: return 1.3f;
            case TerrainType::Snow: return 1.2f;
            case TerrainType::Valley: return 0.8f;
            case TerrainType::Beach: return 1.0f;
            default: return 1.0f;
        }
    }
    int getDefenseBonus(int x, int y) const {
        if (x < 0 || x >= (int)tiles[0].size() || y < 0 || y >= (int)tiles.size()) return 0;
        switch(tiles[y][x]) {
            case TerrainType::Forest: return 2;
            case TerrainType::Mountain: return 5;
            case TerrainType::Rock: return 3;
            case TerrainType::Cliff: return 4;
            default: return 0;
        }
    }
};

struct SpecialAbilityData {
    SpecialAbility ability;
    float cooldown;
    float currentCooldown;
    float duration;
    float range;
    int damage;
    SpecialAbilityData(SpecialAbility a, float cd, float d, float r, int dmg):
        ability(a), cooldown(cd), currentCooldown(0), duration(d), range(r), damage(dmg) {}
};
std::map<UnitType, std::vector<SpecialAbilityData>> unitAbilities;

struct Event {
    std::string name;
    std::string description;
    float triggerTime;
    bool triggered;
    std::function<void()> action;
};
std::vector<Event> events;

struct CampaignMission {
    std::string name;
    std::string description;
    std::string map;
    int difficulty;
    int recommendedUnits;
    std::vector<std::string> objectives;
    bool completed;
    CampaignMission(std::string n, std::string d, std::string m, int dif, int rec, std::vector<std::string> obj):
        name(n), description(d), map(m), difficulty(dif), recommendedUnits(rec), objectives(obj), completed(false) {}
};
std::vector<CampaignMission> campaign = {
    CampaignMission("First Contact", "Defend your base against the initial zerg assault", "desert.map", 1, 10, {"Survive 5 minutes", "Destroy 20 zerglings"}),
    CampaignMission("The Rally", "Gather resources and build your army", "plains.map", 1, 5, {"Collect 2000 minerals", "Build 3 barracks"}),
    CampaignMission("Enemy Territory", "Infiltrate the enemy base", "mountains.map", 2, 20, {"Destroy enemy command center", "Survive"}),
    CampaignMission("The Swarm", "Survive the zerg swarm", "swamp.map", 3, 30, {"Survive 10 waves", "Kill 100 units"}),
    CampaignMission("Air Superiority", "Control the skies", "islands.map", 2, 25, {"Destroy enemy starport", "Build wraiths"}),
    CampaignMission("Ground Assault", "Lead a ground assault on the enemy", "valley.map", 3, 40, {"Destroy all enemy buildings", "No units lost"}),
    CampaignMission("The Protoss Arrival", "Encounter the Protoss empire", "temple.map", 4, 50, {"Defeat the Protoss scouts", "Research dark templar"}),
    CampaignMission("Zerg Horde", "Face the full zerg horde", "nest.map", 4, 60, {"Survive 20 waves", "Destroy Overmind"}),
    CampaignMission("Protoss Awakening", "Engage the full Protoss army", "nexus.map", 5, 70, {"Destroy 5 gateways", "Kill High Templar"}),
    CampaignMission("Final Stand", "The ultimate battle for survival", "arena.map", 5, 100, {"Destroy all enemy bases", "Win the game"})
};

struct InventoryItem {
    std::string name;
    std::string description;
    std::string type;
    int value;
    int quantity;
    bool usable;
    InventoryItem(std::string n, std::string d, std::string t, int v, int q, bool u):
        name(n), description(d), type(t), value(v), quantity(q), usable(u) {}
};
std::vector<InventoryItem> inventory;

struct ChatMessage {
    std::string sender;
    std::string message;
    float timestamp;
    ChatMessage(std::string s, std::string m, float t):sender(s),message(m),timestamp(t){}
};
std::vector<ChatMessage> chatMessages;

struct ConsoleCommand {
    std::string name;
    std::string description;
    std::function<void(std::vector<std::string>)> func;
    ConsoleCommand(std::string n, std::string d, std::function<void(std::vector<std::string>)> f):name(n),description(d),func(f){}
};
std::vector<ConsoleCommand> consoleCommands;

struct NetworkPlayer {
    std::string name;
    int id;
    bool connected;
    Vec2 pos;
    int score;
    NetworkPlayer(std::string n, int i):name(n),id(i),connected(true),score(0){}
};
std::vector<NetworkPlayer> networkPlayers;

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
Terrain terrain;
int camX = 0, camY = 0;
int selectedCount = 0;
GameState gameState = GameState::Playing;
GameMode gameMode = GameMode::Skirmish;
int waveNumber = 0;
float waveTimer = 60.0f;
float gameTime = 0.0f;
bool isPaused = false;
bool debugMode = false;
bool godMode = false;
bool infiniteResources = false;

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
    terrain.generate(MAP_W, MAP_H);
    
    playerResources.minerals = 500;
    playerResources.supplyMax = 20;
    enemyResources.minerals = 500;
    enemyResources.supplyMax = 20;
}

void spawnPowerUp(Vec2 pos) {
    std::string types[] = {"Health", "Shield", "Speed", "Damage", "Resource", "Invulnerability", "Nuke", "Reveal", "Heal", "Repair", "Energy", "ShieldRegen", "AttackSpeed", "MoveSpeed", "Armor", "Vision", "Cloak", "Teleport", "Resurrect", "Barrage"};
    std::string type = types[rand() % 20];
    powerups.emplace_back(pos, type, 30.0f + rand() % 30);
}

void spawnBoss() {
    std::vector<std::string> names = {"Overmind", "Kerrigan", "Artanis", "Amon", "Abathur", "Zagara", "Dehaka", "Nova", "Raynor", "Mengsk", "Fenix", "Aldaris", "Tassadar", "JimRaynor", "SarahKerrigan", "Duran", "SamirDuran", "AlexeiStukov", "Magellan", "Horner"};
    std::string name = names[rand() % 20];
    int health = 500 + waveNumber * 100;
    int damage = 20 + waveNumber * 5;
    Vec2 pos(275 + rand() % 5, 275 + rand() % 5);
    bosses.emplace_back(name, health, damage, 8, 2.0f, pos, "boss");
}

void spawnEliteEnemy() {
    std::string names[] = {"EliteMarine", "EliteTank", "EliteZergling", "EliteUltralisk", "EliteGhost", "EliteBattlecruiser", "EliteZealot", "EliteDragoon"};
    std::string name = names[rand() % 8];
    Vec2 pos(275 + rand() % 5, 275 + rand() % 5);
    UnitType type;
    switch(rand() % 8) {
        case 0: type = UnitType::Marine; break;
        case 1: type = UnitType::Tank; break;
        case 2: type = UnitType::Zergling; break;
        case 3: type = UnitType::Ultralisk; break;
        case 4: type = UnitType::Ghost; break;
        case 5: type = UnitType::Battlecruiser; break;
        case 6: type = UnitType::Zealot; break;
        case 7: type = UnitType::Dragoon; break;
    }
    Unit u(pos, type, Team::Enemy);
    u.maxHealth *= 2;
    u.health = u.maxHealth;
    u.attackPower = (int)(u.attackPower * 1.5);
    units.push_back(u);
}

struct Map {
    std::string name;
    std::string description;
    int width;
    int height;
    int resourceDensity;
    int enemyDensity;
    std::vector<std::string> features;
    Map(std::string n, std::string d, int w, int h, int rd, int ed, std::vector<std::string> f):
        name(n), description(d), width(w), height(h), resourceDensity(rd), enemyDensity(ed), features(f) {}
};
std::vector<Map> maps = {
    Map("Plains", "Open terrain with scattered resources", 300, 300, 20, 10, {"grass", "minerals"}),
    Map("Desert", "Arid wasteland with limited water", 400, 400, 15, 15, {"sand", "rocks", "cacti"}),
    Map("Mountains", "Rugged terrain with defensive positions", 350, 350, 18, 12, {"peaks", "valleys", "cliffs"}),
    Map("Islands", "Archipelago with water separating land masses", 500, 500, 25, 8, {"water", "beaches"}),
    Map("Swamp", "Muddy terrain that slows movement", 400, 400, 22, 18, {"water", "trees", "fog"}),
    Map("Arctic", "Frozen tundra with limited resources", 450, 450, 10, 20, {"ice", "snow", "blizzard"}),
    Map("Jungle", "Dense forest with rich resources", 350, 350, 30, 15, {"trees", "vines", "wildlife"}),
    Map("Volcanic", "Active volcanic region with lava hazards", 400, 400, 15, 25, {"lava", "ash", "geysers"}),
    Map("Urban", "Ruined cityscape with many structures", 500, 500, 20, 30, {"buildings", "ruins", "debris"}),
    Map("Space", "Orbital platform with zero gravity", 600, 600, 25, 35, {"asteroids", "debris", "radiation"}),
    Map("Underground", "Cave system with narrow passages", 300, 300, 18, 22, {"tunnels", "caverns", "rivers"}),
    Map("Coastal", "Beaches and shallow waters", 400, 400, 22, 12, {"beach", "reef", "docks"}),
    Map("Tundra", "Frozen plains with permafrost", 450, 450, 12, 18, {"snow", "ice", "aurora"}),
    Map("Savanna", "Grassy plains with scattered trees", 500, 500, 28, 10, {"grass", "acacia", "waterholes"}),
    Map("Canyon", "Deep ravines and rocky outcrops", 350, 350, 15, 20, {"cliffs", "rivers", "caves"}),
    Map("Marsh", "Wetlands with tall reeds", 400, 400, 20, 16, {"water", "reeds", "islands"}),
    Map("Forest", "Dense woodland with hidden paths", 450, 450, 25, 14, {"trees", "clearings", "streams"}),
    Map("Prairie", "Endless grasslands with few trees", 600, 600, 15, 8, {"grass", "wildflowers", "hills"}),
    Map("Oasis", "Desert haven with abundant water", 300, 300, 35, 5, {"palms", "water", "sand"}),
    Map("Crater", "Impact zone with destroyed ships", 400, 400, 18, 28, {"crater", "debris", " wreckage"}),
    Map("Highlands", "Mountain peaks with strategic advantage", 450, 450, 20, 22, {"peaks", "cliffs", "wind"}),
    Map("Wasteland", "Radioactive ruins of civilization", 500, 500, 10, 30, {"radiation", "ruins", "mutants"}),
    Map("Archipelago", "Chain of small islands", 600, 600, 22, 10, {"islands", "reefs", "beaches"}),
    Map("Glacier", "Massive ice formations", 550, 550, 8, 15, {"ice", "crevasses", "cold"}),
    Map("Reef", "Underwater coral formations", 400, 400, 30, 20, {"coral", "fish", "caves"})
};

struct AchievementTier {
    std::string name;
    std::vector<Achievement> achievements;
    bool unlocked = false;
    AchievementTier(std::string n, std::vector<Achievement> a): name(n), achievements(a) {}
};
std::vector<AchievementTier> achievementTiers = {
    AchievementTier("Bronze", {achievements[0], achievements[1], achievements[2]}),
    AchievementTier("Silver", {achievements[3], achievements[4], achievements[5]}),
    AchievementTier("Gold", {achievements[6], achievements[7], achievements[8]}),
    AchievementTier("Platinum", {achievements[9], achievements[10], achievements[11]}),
    AchievementTier("Diamond", {achievements[12], achievements[13], achievements[14]}),
    AchievementTier("Master", {achievements[15], achievements[16], achievements[17]}),
    AchievementTier("Grandmaster", {achievements[18], achievements[19], achievements[20]})
};

struct DailyChallenge {
    std::string name;
    std::string description;
    std::string map;
    int difficulty;
    std::vector<std::string> modifiers;
    DailyChallenge(std::string n, std::string d, std::string m, int dif, std::vector<std::string> mod):
        name(n), description(d), map(m), difficulty(dif), modifiers(mod) {}
};
std::vector<DailyChallenge> dailyChallenges = {
    DailyChallenge("Speed Run", "Defeat all enemies in under 5 minutes", "plains", 3, {"fastEnemies", "slowPlayer", "noPause"}),
    DailyChallenge("Resource Rush", "Win with minimal resources", "desert", 4, {"lowResources", "fastEnemies", "noRepair"}),
    DailyChallenge("Last Stand", "Survive as long as possible", "urban", 5, {"endlessWaves", "strongEnemies", "noRecovery"}),
    DailyChallenge("No Buildings", "Win without constructing buildings", "plains", 3, {"noBuilding", "startingUnits", "speedBonus"}),
    DailyChallenge("Glass Cannon", "Win using only damage units", "mountains", 4, {"noTanks", "highDamage", "lowHealth"}),
    DailyChallenge("Defensive Master", "Win using only defensive structures", "islands", 3, {"onlyTurrets", "noAttack", "strongWalls"}),
    DailyChallenge("Air Superiority", "Win using only air units", "archipelago", 4, {"onlyAir", "antiAirEnemies", "highAltitude"}),
    DailyChallenge("Zerg Rush", "Win using only zerg units", "jungle", 3, {"onlyZerg", "fastSpawn", "swarmBonus"}),
    DailyChallenge("Terran Tech", "Win using only terran units", "urban", 4, {"onlyTerran", "mechOnly", "techBonus"}),
    DailyChallenge("Protoss Precision", "Win using only protoss units", "highlands", 5, {"onlyProtoss", "highTech", "shieldBonus"}),
    DailyChallenge("Mixed Fleet", "Win using all unit types", "space", 5, {"allUnits", "coordination", "versatility"}),
    DailyChallenge("Nightmare Mode", "Survive the toughest challenges", "wasteland", 10, {"allMods", "bossRush", "noMercy"})
};

struct WeeklyChallenge {
    std::string name;
    std::string description;
    int difficulty;
    std::vector<std::string> rewards;
    WeeklyChallenge(std::string n, std::string d, int dif, std::vector<std::string> r):
        name(n), description(d), difficulty(dif), rewards(r) {}
};
std::vector<WeeklyChallenge> weeklyChallenges = {
    WeeklyChallenge("Operation Sunrise", "Liberate the occupied territory", 5, {"medal", "credits", "skins"}),
    WeeklyChallenge("Operation Midnight", "Strike under cover of darkness", 6, {"medal", "credits", "units"}),
    WeeklyChallenge("Operation Thunder", "Destroy the enemy command", 7, {"medal", "credits", "buildings"}),
    WeeklyChallenge("Operation Blizzard", "Survive the frozen wasteland", 8, {"medal", "credits", "terrain"}),
    WeeklyChallenge("Operation Inferno", "Conquer the volcanic region", 9, {"medal", "credits", "damage"}),
    WeeklyChallenge("Operation Storm", "Weather the cosmic storm", 10, {"medal", "credits", "all"}),
    WeeklyChallenge("Operation Unity", "Unite all factions", 10, {"medal", "credits", "allUnits", "allBuildings"})
};

struct Tournament {
    std::string name;
    std::vector<std::string> participants;
    int currentRound;
    std::string prize;
    bool completed;
    Tournament(std::string n, int rounds, std::string p):
        name(n), currentRound(0), prize(p), completed(false) {}
};
std::vector<Tournament> tournaments;

struct Season {
    std::string name;
    int number;
    std::string startDate;
    std::string endDate;
    std::vector<std::string> rewards;
    bool active;
    Season(std::string n, int num, std::string s, std::string e, std::vector<std::string> r):
        name(n), number(num), startDate(s), endDate(e), rewards(r), active(true) {}
};
std::vector<Season> seasons = {
    Season("Season 1: Awakening", 1, "2026-01-01", "2026-03-31", {"bronze_skin", "silver_skin", "gold_skin"}),
    Season("Season 2: Ascension", 2, "2026-04-01", "2026-06-30", {"epic_skin", "legendary_skin", "mythic_skin"}),
    Season("Season 3: Dominion", 3, "2026-07-01", "2026-09-30", {"unique_skin", "rare_skin", "epic_skin"}),
    Season("Season 4: Reckoning", 4, "2026-10-01", "2026-12-31", {"legendary_skin", "mythic_skin", "ultimate_skin"})
};

void applyPowerUp(Unit& u, PowerUp& p) {
    if (p.type == "Health") u.health = std::min(u.maxHealth, u.health + 50);
    else if (p.type == "Shield") u.maxHealth += 20;
    else if (p.type == "Speed") u.attackCooldown -= 0.2f;
    else if (p.type == "Damage") u.attackPower += 5;
    else if (p.type == "Resource") playerResources.minerals += 100;
    else if (p.type == "Invulnerability") u.alive = true;
    p.active = false;
}

void triggerEvent(Event& e) {
    if (e.triggered) return;
    e.triggered = true;
    e.action();
}

void spawnWeather() {
    std::string weathers[] = {"Clear", "Rain", "Storm", "Fog", "Snow", "Sandstorm", "Nuclear Fallout"};
    weather.currentWeather = weathers[rand() % 7];
    weather.intensity = 0.5f + (rand() % 50) / 100.0f;
    weather.duration = 30.0f + (rand() % 120);
    weather.timer = weather.duration;
    weather.active = true;
}

void updateDayNight(float dt) {
    dayNight.timeOfDay += dt / dayNight.cycleDuration;
    if (dayNight.timeOfDay >= 1.0f) dayNight.timeOfDay -= 1.0f;
    dayNight.isDay = dayNight.timeOfDay < 0.25f || dayNight.timeOfDay > 0.75f;
    dayNight.lightLevel = dayNight.isDay ? 1.0f : 0.3f + 0.3f * std::sin(dayNight.timeOfDay * 3.14159f * 2);
}

void unlockAchievement(std::string name) {
    for (auto& a : achievements) {
        if (a.name == name && !a.unlocked) {
            a.unlocked = true;
        }
    }
}

void checkAchievements() {
    if (stats.unitsKilled >= 1) unlockAchievement("First Blood");
    if (waveNumber >= 1) unlockAchievement("Rusher");
    if (waveNumber >= 5) unlockAchievement("Veteran");
    if (waveNumber >= 10) unlockAchievement("Elite");
    if (waveNumber >= 20) unlockAchievement("Legendary");
    if (stats.resourcesGathered >= 1000) unlockAchievement("Economist");
    if (stats.resourcesGathered >= 5000) unlockAchievement("Wealthy");
    if (stats.buildingsBuilt >= 10) unlockAchievement("Builder");
    if (stats.buildingsBuilt >= 25) unlockAchievement("Constructor");
    if (stats.unitsBuilt >= 50) unlockAchievement("Army Builder");
    if (stats.unitsBuilt >= 100) unlockAchievement("Horde");
    if (stats.buildingsDestroyed >= 10) unlockAchievement("Destroyer");
    if (stats.buildingsDestroyed >= 25) unlockAchievement("Raider");
    if (stats.buildingsDestroyed >= 50) unlockAchievement("Overwhelming Force");
}

void addParticle(Vec2 pos, Vec2 vel, float lifetime, char glyph) {
    particles.emplace_back(pos, vel, lifetime, glyph);
}

void updateParticles(float dt) {
    for (auto& p : particles) {
        if (!p.active) continue;
        p.age += dt;
        if (p.age >= p.lifetime) {
            p.active = false;
            continue;
        }
        p.pos = p.pos + p.vel;
        addParticle(p.pos, Vec2(rand()%3-1, rand()%3-1), 0.5f, '*');
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p){ return !p.active; }), particles.end());
}

void addInventoryItem(InventoryItem item) {
    for (auto& i : inventory) {
        if (i.name == item.name) {
            i.quantity += item.quantity;
            return;
        }
    }
    inventory.push_back(item);
}

void useInventoryItem(int index) {
    if (index < 0 || index >= (int)inventory.size()) return;
    InventoryItem& item = inventory[index];
    if (!item.usable) return;
    if (item.type == "Consumable") {
        if (item.name == "Health Pack") {
            for (auto& u : units) {
                if (u.team == Team::Player && u.alive) {
                    u.health = std::min(u.maxHealth, u.health + item.value);
                }
            }
        } else if (item.name == "Mineral Pack") {
            playerResources.minerals += item.value;
        }
        item.quantity--;
        if (item.quantity <= 0) {
            inventory.erase(inventory.begin() + index);
        }
    }
}

void sendChatMessage(std::string sender, std::string message) {
    chatMessages.emplace_back(sender, message, gameTime);
    if ((int)chatMessages.size() > 100) {
        chatMessages.erase(chatMessages.begin());
    }
}

void executeConsoleCommand(std::string cmd) {
    std::istringstream iss(cmd);
    std::string name;
    iss >> name;
    for (auto& c : consoleCommands) {
        if (c.name == name) {
            std::vector<std::string> args;
            std::string arg;
            while (iss >> arg) args.push_back(arg);
            c.func(args);
            return;
        }
    }
}

void addUnitAbilities() {
    unitAbilities[UnitType::Marine] = {SpecialAbilityData(SpecialAbility::Stim, 30.0f, 0, 0, 0)};
    unitAbilities[UnitType::Medic] = {SpecialAbilityData(SpecialAbility::Heal, 15.0f, 5.0f, 5, 10)};
    unitAbilities[UnitType::Ghost] = {SpecialAbilityData(SpecialAbility::Cloak, 60.0f, 0, 0, 0), SpecialAbilityData(SpecialAbility::EMP, 45.0f, 0, 8, 50)};
    unitAbilities[UnitType::Tank] = {SpecialAbilityData(SpecialAbility::SiegeMode, 20.0f, 0, 0, 0)};
    unitAbilities[UnitType::Battlecruiser] = {SpecialAbilityData(SpecialAbility::Yamato, 90.0f, 0, 15, 200)};
    unitAbilities[UnitType::ScienceVessel] = {SpecialAbilityData(SpecialAbility::Irradiate, 45.0f, 0, 10, 100), SpecialAbilityData(SpecialAbility::DefensiveMatrix, 60.0f, 0, 8, 0)};
    unitAbilities[UnitType::Firebat] = {SpecialAbilityData(SpecialAbility::Stim, 30.0f, 0, 0, 0)};
    unitAbilities[UnitType::Wraith] = {SpecialAbilityData(SpecialAbility::Cloak, 60.0f, 0, 0, 0)};
    unitAbilities[UnitType::DarkTemplar] = {SpecialAbilityData(SpecialAbility::Cloak, 30.0f, 0, 0, 0)};
    unitAbilities[UnitType::HighTemplar] = {SpecialAbilityData(SpecialAbility::PsionicStorm, 90.0f, 0, 12, 150), SpecialAbilityData(SpecialAbility::Feedback, 45.0f, 0, 8, 0)};
    unitAbilities[UnitType::Queen] = {SpecialAbilityData(SpecialAbility::SpawnBroodlings, 120.0f, 0, 10, 0), SpecialAbilityData(SpecialAbility::Ensnare, 60.0f, 0, 10, 0)};
    unitAbilities[UnitType::Defiler] = {SpecialAbilityData(SpecialAbility::DarkSwarm, 90.0f, 0, 12, 0), SpecialAbilityData(SpecialAbility::Plague, 120.0f, 0, 10, 100), SpecialAbilityData(SpecialAbility::Consume, 30.0f, 0, 3, 0)};
    unitAbilities[UnitType::Lurker] = {SpecialAbilityData(SpecialAbility::Burrow, 20.0f, 0, 0, 0)};
    unitAbilities[UnitType::Zergling] = {SpecialAbilityData(SpecialAbility::Burrow, 15.0f, 0, 0, 0)};
    unitAbilities[UnitType::Ultralisk] = {SpecialAbilityData(SpecialAbility::Burrow, 25.0f, 0, 0, 0)};
    unitAbilities[UnitType::Archon] = {SpecialAbilityData(SpecialAbility::PsionicStorm, 90.0f, 0, 12, 150)};
    unitAbilities[UnitType::Arbiter] = {SpecialAbilityData(SpecialAbility::StasisField, 90.0f, 0, 10, 0), SpecialAbilityData(SpecialAbility::PhaseShift, 60.0f, 0, 8, 0)};
    unitAbilities[UnitType::Ghost] = {SpecialAbilityData(SpecialAbility::Lockdown, 90.0f, 0, 10, 0), SpecialAbilityData(SpecialAbility::EMP, 45.0f, 0, 8, 50)};
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
            break;
        }
        case 'e': case 'E': {
            if (playerResources.minerals >= 150) {
                Building* cc = findNearestBuilding(Vec2(camX + COLS/2, camY + LINES/2), BuildingType::CommandCentre, Team::Player);
                if (cc) {
                    Vec2 pos(cc->pos.x + 5, cc->pos.y + 5);
                    if (!isOccupied(pos)) {
                        buildings.emplace_back(pos, BuildingType::Barracks, Team::Player);
                        playerResources.minerals -= 150;
                        stats.buildingsBuilt++;
                    }
                }
            }
            break;
        }
        case 'f': case 'F': {
            if (playerResources.minerals >= 200) {
                Building* cc = findNearestBuilding(Vec2(camX + COLS/2, camY + LINES/2), BuildingType::CommandCentre, Team::Player);
                if (cc) {
                    Vec2 pos(cc->pos.x + 8, cc->pos.y + 5);
                    if (!isOccupied(pos)) {
                        buildings.emplace_back(pos, BuildingType::Factory, Team::Player);
                        playerResources.minerals -= 200;
                        stats.buildingsBuilt++;
                    }
                }
            }
            break;
        }
        case 'o': case 'O': {
            if (playerResources.minerals >= 250) {
                Building* cc = findNearestBuilding(Vec2(camX + COLS/2, camY + LINES/2), BuildingType::CommandCentre, Team::Player);
                if (cc) {
                    Vec2 pos(cc->pos.x + 11, cc->pos.y + 5);
                    if (!isOccupied(pos)) {
                        buildings.emplace_back(pos, BuildingType::Starport, Team::Player);
                        playerResources.minerals -= 250;
                        stats.buildingsBuilt++;
                    }
                }
            }
            break;
        }
        case 'i': case 'I': {
            if (playerResources.minerals >= 100) {
                Building* cc = findNearestBuilding(Vec2(camX + COLS/2, camY + LINES/2), BuildingType::CommandCentre, Team::Player);
                if (cc) {
                    Vec2 pos(cc->pos.x - 5, cc->pos.y);
                    if (!isOccupied(pos)) {
                        buildings.emplace_back(pos, BuildingType::SupplyDepot, Team::Player);
                        playerResources.minerals -= 100;
                        playerResources.supplyMax += 8;
                        stats.buildingsBuilt++;
                    }
                }
            }
            break;
        }
        case 'u': case 'U': {
            if (playerResources.minerals >= 100) {
                Building* cc = findNearestBuilding(Vec2(camX + COLS/2, camY + LINES/2), BuildingType::CommandCentre, Team::Player);
                if (cc) {
                    Vec2 pos(cc->pos.x - 5, cc->pos.y + 5);
                    if (!isOccupied(pos)) {
                        buildings.emplace_back(pos, BuildingType::Refinery, Team::Player);
                        playerResources.minerals -= 100;
                        stats.buildingsBuilt++;
                    }
                }
            }
            break;
        }
        case 'k': case 'K': {
            if (playerResources.minerals >= 75) {
                Building* cc = findNearestBuilding(Vec2(camX + COLS/2, camY + LINES/2), BuildingType::CommandCentre, Team::Player);
                if (cc) {
                    Vec2 pos(cc->pos.x + 3, cc->pos.y - 3);
                    if (!isOccupied(pos)) {
                        buildings.emplace_back(pos, BuildingType::Turret, Team::Player);
                        playerResources.minerals -= 75;
                        stats.buildingsBuilt++;
                    }
                }
            }
            break;
        }
        case 'j': case 'J': {
            if (playerResources.minerals >= 100) {
                Building* cc = findNearestBuilding(Vec2(camX + COLS/2, camY + LINES/2), BuildingType::CommandCentre, Team::Player);
                if (cc) {
                    Vec2 pos(cc->pos.x + 3, cc->pos.y + 3);
                    if (!isOccupied(pos)) {
                        buildings.emplace_back(pos, BuildingType::Bunker, Team::Player);
                        playerResources.minerals -= 100;
                        stats.buildingsBuilt++;
                    }
                }
            }
            break;
        }
        case 'l': case 'L': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Medic, Team::Player);
                    playerResources.minerals -= 100;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'y': case 'Y': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Tank, Team::Player);
                    playerResources.minerals -= 300;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'h': case 'H': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Fighter, Team::Player);
                    playerResources.minerals -= 250;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'n': case 'N': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Ghost, Team::Player);
                    playerResources.minerals -= 200;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'g': case 'G': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Goliath, Team::Player);
                    playerResources.minerals -= 250;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'x': case 'X': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Firebat, Team::Player);
                    playerResources.minerals -= 150;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'z': case 'Z': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Wraith, Team::Player);
                    playerResources.minerals -= 200;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'v': case 'V': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::ScienceVessel, Team::Player);
                    playerResources.minerals -= 350;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 'b': case 'B': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Battlecruiser, Team::Player);
                    playerResources.minerals -= 600;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case '/': case '?': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Dropship, Team::Player);
                    playerResources.minerals -= 250;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case 't': case 'T': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Vulture, Team::Player);
                    playerResources.minerals -= 150;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case ';': case ':': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Corsair, Team::Player);
                    playerResources.minerals -= 200;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case '\'': case '"': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Scout, Team::Player);
                    playerResources.minerals -= 100;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case '[': case '{': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::DarkTemplar, Team::Player);
                    playerResources.minerals -= 250;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case ']': case '}': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Dragoon, Team::Player);
                    playerResources.minerals -= 200;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case '\\': case '|': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Zealot, Team::Player);
                    playerResources.minerals -= 150;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case '.': case '>': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Carrier, Team::Player);
                    playerResources.minerals -= 550;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case ',': case '<': {
            for (auto& u : units) {
                if (u.selected && u.team == Team::Player && u.alive) {
                    units.emplace_back(Vec2(u.pos.x + 1, u.pos.y + 1), UnitType::Reaver, Team::Player);
                    playerResources.minerals -= 400;
                    stats.unitsBuilt++;
                }
            }
            break;
        }
        case '`': case '~': {
            if (debugMode) {
                godMode = !godMode;
                infiniteResources = !infiniteResources;
            }
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
    addUnitAbilities();
    
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
