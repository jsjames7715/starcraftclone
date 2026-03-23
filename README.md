# StarCraft Clone

A terminal-based RTS game inspired by StarCraft, built with C++ and ncurses.

## Features

### Core Gameplay
- **26 Unit Types**: Workers, Marines, Medics, Tanks, Fighters, Zerglings, Ultralisks, Ghost, Vulture, Goliath, Wraith, Battlecruiser, Science Vessel, Firebat, Dropship, Valkyrie, Corsair, Scout, Dark Templar, Dragoon, Zealot, Archon, Carrier, Reaver, Shuttle
- **21 Building Types**: Command Centre, Barracks, Factory, Starport, Supply Depot, Refinery, Turret, Bunker, Academy, Armory, Tech Lab, ComSat, Nuclear Silo, Pylon, Gateway, Cybernetics Core, Stargate, Fleet Beacon, Robotics Bay, Observatory
- **Resource Gathering**: Collect minerals with workers
- **Tech Tree**: Build advanced structures to unlock new units
- **Combat System**: Ranged attacks, armor, health bars
- **Wave Defense**: Survive increasingly difficult enemy waves

### AI Systems
- **Enemy AI**: Automatic pathfinding toward player base
- **Medic Healing**: Medics automatically heal nearby units
- **Wave Spawning**: Progressive difficulty with varied enemy compositions

### Visual Features
- **Scrolling Map**: 300x300 tile explorable terrain
- **Fog of War**: Visibility system reveals only explored areas
- **Health Bars**: Visual feedback for damaged units/buildings
- **Minimap**: Overview of the battlefield
- **HUD**: Real-time stats display

### Game Modes
- **Campaign**: Story-driven missions
- **Skirmish**: Defend against enemy waves
- **Deathmatch**: All-out warfare with no objectives
- **Survival**: Endless waves, how long can you last?
- **Rush**: Start with resources, overwhelm the enemy quickly
- **Economic**: Focus on building your economy
- **Tower Defense**: Defend strategic points
- **Capture The Flag**: Steal the enemy flag
- **King of the Hill**: Control key positions
- **Custom**: Configure your own game settings

## Controls

| Key | Action |
|-----|--------|
| WASD / Arrow Keys | Pan camera |
| Left Click | Select unit/building |
| Right Click | Move selected units |
| 1-9 | Quick select units |
| s | Train worker (Command Centre) |
| t | Train marine (Barracks) |
| b | Build barracks |
| c | Build command centre |
| p | Pause game |
| q | Quit |

## Building Requirements

| Building | Cost | Produces |
|----------|------|----------|
| Command Centre | 400 | Workers |
| Barracks | 150 | Marines, Medics |
| Factory | 200 | Tanks |
| Starport | 250 | Fighters |
| Supply Depot | 100 | +10 supply |
| Refinery | 100 | Gas extraction |
| Turret | 75 | Defense |
| Bunker | 100 | Infantry shelter |
| Academy | 150 | Advanced units |
| Armory | 200 | Upgrades |
| Tech Lab | 150 | Special research |
| ComSat | 150 | Scanner sweep |
| Nuclear Silo | 200 | Nuke |
| Pylon | 100 | Protoss power |
| Gateway | 150 | Protoss units |
| Cybernetics Core | 200 | Advanced protoss |
| Stargate | 250 | Air units |
| Fleet Beacon | 200 | Carriers |
| Robotics Bay | 250 | Heavy units |
| Observatory | 150 | Scout |

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
│   ├── main.cpp      # Main game logic (C++ RTS engine)
│   ├── game.js        # JavaScript game engine
│   ├── utils.js       # Utility functions
│   └── main.js        # Entry point
├── assets/
│   └── models/        # 3D models (.obj, .dae)
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
