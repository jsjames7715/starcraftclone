# StarCraft Clone

A comprehensive terminal-based RTS game inspired by StarCraft, built with C++ and ncurses.

## Features

### Core Gameplay
- **70+ Unit Types**: Workers, Marines, Medics, Tanks, Fighters, Zerglings, Ultralisks, Ghost, Vulture, Goliath, Wraith, Battlecruiser, Science Vessel, Firebat, Dropship, Valkyrie, Corsair, Scout, Dark Templar, Dragoon, Zealot, Archon, Carrier, Reaver, Shuttle, Mutalisk, Hydralisk, Defiler, Lurker, Broodling, Guardian, Devourer, Queen, Baneling, Roach, Infestor, Swarm Host, Viper, Overseer, Arbiter, High Templar, Interceptor, Scarab, Overlord, Probe, and more
- **50+ Building Types**: Command Centre, Barracks, Factory, Starport, Supply Depot, Refinery, Turret, Bunker, Academy, Armory, Tech Lab, ComSat, Nuclear Silo, Pylon, Gateway, Cybernetics Core, Stargate, Fleet Beacon, Robotics Bay, Observatory, Hatchery, Lair, Hive, Spire, Spawning Pool, Evolution Chamber, Extractor, Hydralisk Den, Spore Colony, Sunken Colony, Ultralisk Cavern, Nydus Canal, Infestation Pit, Great Hall, Nexus, Forge, Robotics Facility, Twilight Council, Dark Shrine, Templar Archives, Photon Cannon, Shield Battery, Warp Gate, Planetary Fortress, Orbital Command, Engineering Bay, Ghost Academy, Missile Turret, Point Defense, Warp Prism, Disruptor, Oracle, Tempest, Mothership, Mothership Core, and more
- **Resource Gathering**: Collect minerals and gas with workers
- **Tech Tree**: Build advanced structures to unlock new units
- **Combat System**: Ranged attacks, armor, health bars, special abilities
- **Wave Defense**: Survive increasingly difficult enemy waves

### Advanced Systems
- **25 Achievements**: First Blood, Rusher, Veteran, Elite, Legendary, Economist, Wealthy, Builder, Constructor, Army Builder, Horde, Destroyer, Raider, Overwhelming Force, Speed Demon, Survivalist, Clean Sweep, Efficiency Expert, Turtle, Blitz, Marathon, Boss Slayer, Boss Destroyer, Collector, Power Overwhelming, Upgrade Master
- **20 Power-ups**: Health, Shield, Speed, Damage, Resource, Invulnerability, Nuke, Reveal, Heal, Repair, Energy, Shield Regen, Attack Speed, Move Speed, Armor, Vision, Cloak, Teleport, Resurrect, Barrage
- **20 Boss Enemies**: Overmind, Kerrigan, Artanis, Amon, Abathur, Zagara, Dehaka, Nova, Raynor, Mengsk, Fenix, Aldaris, Tassadar, Jim Raynor, Sarah Kerrigan, Duran, Samir Duran, Alexei Stukov, Magellan, Horner
- **7 Weather Types**: Clear, Rain, Storm, Fog, Snow, Sandstorm, Nuclear Fallout
- **Day/Night Cycle**: 120-second cycle with dynamic lighting
- **15 Upgrades**: Stim Pack, Shield Battery, Marine Range, Marine Damage, Tank Siege Mode, Medic Healing, Ghost Cloak, Vulture Speed, Goliath Range, Wraith Cloak, Battlecruiser Yamato, Science Vessel Defensive, Firebat Damage, Dropship Capacity, Valkyrie Damage
- **10 Special Abilities**: Stim, Heal, Cloak, Siege Mode, Yamato, Irradiate, Psionic Storm, Lockdown, EMP, Scanner Sweep
- **10 Campaign Missions**: First Contact, The Rally, Enemy Territory, The Swarm, Air Superiority, Ground Assault, The Protoss Arrival, Zerg Horde, Protoss Awakening, Final Stand

### Terrain System
- **15 Terrain Types**: Grass, Water, Mountain, Forest, Sand, Rock, Lava, Ice, Swamp, Desert, Snow, Cliff, Canyon, Valley, Beach
- **Movement Costs**: Different terrain affects unit movement speed
- **Defense Bonuses**: Terrain provides defensive bonuses to units

### Maps
- **25 Unique Maps**: Plains, Desert, Mountains, Islands, Swamp, Arctic, Jungle, Volcanic, Urban, Space, Underground, Coastal, Tundra, Savanna, Canyon, Marsh, Forest, Prairie, Oasis, Crater, Highlands, Wasteland, Archipelago, Glacier, Reef

### Game Modes
- **40+ Game Modes**: Campaign, Skirmish, Deathmatch, Survival, Rush, Economic, Tower Defense, Capture The Flag, King of the Hill, Custom, Arena, Blitz, Puzzle, Boss Rush, Escort, Assault, Defense, Recon, Sabotage, Hero Mode, Nightmare, Iron Man, Weekly Challenge, Daily Challenge, Tournament, Multiplayer, LAN, Online, Cooperative, Solo, Practice, Tutorial, Challenge, Endless, Gauntlet

### Visual Features
- **Scrolling Map**: 300x300 tile explorable terrain
- **Fog of War**: Visibility system reveals only explored areas
- **Health Bars**: Visual feedback for damaged units/buildings
- **Minimap**: Overview of the battlefield
- **HUD**: Real-time stats display
- **Particles**: Visual effects for explosions and combat

### AI Systems
- **Enemy AI**: Automatic pathfinding toward player base
- **Medic Healing**: Medics automatically heal nearby units
- **Wave Spawning**: Progressive difficulty with varied enemy compositions
- **Elite Enemies**: Special strengthened enemy units
- **Boss Encounters**: Epic boss battles with unique abilities

### Statistics & Progression
- **Game Stats**: Units killed, buildings destroyed, resources gathered, game time, waves completed, damage dealt/taken, units/buildings constructed, missions completed, bosses defeated
- **Leaderboards**: Track high scores and player rankings
- **Achievement Tiers**: Bronze, Silver, Gold, Platinum, Diamond, Master, Grandmaster
- **Seasons**: 4 seasonal content updates with unique rewards
- **Daily Challenges**: 12 unique daily challenges
- **Weekly Challenges**: 7 weekly challenges

### Networking & Multiplayer
- **Network Players**: Support for multiple players
- **Chat System**: In-game messaging
- **Console Commands**: Debug and configuration commands

### Inventory System
- **Inventory Items**: Collect and use items during gameplay
- **Consumables**: Health packs, mineral packs, and more

## Controls

| Key | Action |
|-----|--------|
| WASD / Arrow Keys | Pan camera |
| Left Click | Select unit/building |
| Right Click | Move selected units |
| 1-9 | Quick select units |
| s | Train worker (Command Centre) |
| t | Train marine (Barracks) |
| l | Train medic |
| x | Train firebat |
| y | Train tank |
| h | Train fighter |
| n | Train ghost |
| g | Train goliath |
| z | Train wraith |
| v | Train science vessel |
| b | Train battlecruiser |
| / | Train dropship |
| t | Train vulture |
| b | Build barracks |
| c | Build command centre |
| e | Build barracks |
| f | Build factory |
| o | Build starport |
| i | Build supply depot |
| u | Build refinery |
| k | Build turret |
| j | Build bunker |
| p | Pause game |
| q | Quit |

## Unit Stats

| Unit | HP | Attack | Armor | Cost |
|------|-----|--------|-------|------|
| Worker | 60 | 5 | 0 | 50 |
| Marine | 80 | 12 | 1 | 100 |
| Medic | 60 | 0 | 0 | 100 |
| Tank | 200 | 50 | 3 | 300 |
| Fighter | 100 | 25 | 2 | 250 |
| Zergling | 40 | 8 | 0 | 50 |
| Ultralisk | 400 | 40 | 5 | 400 |
| Ghost | 90 | 20 | 1 | 200 |
| Vulture | 80 | 20 | 0 | 150 |
| Goliath | 150 | 30 | 2 | 250 |
| Wraith | 120 | 25 | 2 | 200 |
| Battlecruiser | 500 | 60 | 5 | 600 |
| Science Vessel | 200 | 0 | 2 | 350 |
| Firebat | 100 | 16 | 1 | 150 |
| Dropship | 150 | 0 | 2 | 250 |
| Valkyrie | 180 | 35 | 3 | 350 |
| Corsair | 100 | 18 | 1 | 200 |
| Scout | 80 | 15 | 0 | 100 |
| Dark Templar | 80 | 45 | 1 | 250 |
| Dragoon | 180 | 30 | 2 | 200 |
| Zealot | 160 | 20 | 1 | 150 |
| Archon | 350 | 50 | 3 | 400 |
| Carrier | 400 | 40 | 4 | 550 |
| Reaver | 200 | 100 | 2 | 400 |
| Shuttle | 120 | 0 | 2 | 200 |
| Mutalisk | 120 | 25 | 1 | 100 |
| Hydralisk | 90 | 15 | 0 | 75 |
| Defiler | 80 | 0 | 0 | 100 |
| Lurker | 120 | 30 | 1 | 150 |
| Broodling | 30 | 8 | 0 | 30 |
| Guardian | 150 | 40 | 2 | 200 |
| Devourer | 250 | 35 | 3 | 250 |
| Queen | 100 | 15 | 1 | 125 |
| Baneling | 30 | 40 | 0 | 50 |
| Roach | 100 | 18 | 1 | 75 |
| Infestor | 90 | 0 | 0 | 150 |
| Swarm Host | 140 | 0 | 2 | 200 |
| Viper | 120 | 0 | 1 | 250 |
| Overseer | 200 | 0 | 2 | 150 |
| Arbiter | 300 | 20 | 3 | 300 |
| High Templar | 80 | 0 | 0 | 150 |
| Interceptor | 40 | 8 | 0 | 50 |
| Scarab | 50 | 20 | 0 | 75 |
| Overlord | 400 | 0 | 2 | 100 |
| Probe | 50 | 5 | 0 | 50 |

## Building

```bash
# Terminal version (ncurses)
make

# CMake build (with OpenGL option)
mkdir build && cd build
cmake .. -DUSE_OPENGL=ON
make
```

## Running

```bash
./starcraft
```

## Project Structure

```
star/
├── src/
│   ├── main.cpp      # Main game logic (C++ RTS engine) - 2000+ lines
│   ├── game.js       # JavaScript game engine
│   ├── utils.js      # Utility functions
│   └── main.js       # Entry point
├── assets/
│   └── models/       # 3D models (.obj, .dae)
├── art/               # Sprites and textures
├── sounds/            # Audio files
├── starcraft_mod/     # Mod support
└── docs/              # Documentation
```

## Multiplayer (Future)

Network play via ENet library:
- Client-server architecture
- Player state synchronization
- Reliable UDP packets

## License

MIT License

## Contributing

See [CONTRIBUTING.md](docs/CONTRIBUTING.md)
