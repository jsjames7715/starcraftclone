# StarCraft Clone

A terminal-based RTS game inspired by StarCraft, built with C++ and ncurses.

## Features

### Core Gameplay
- **Multiple Unit Types**: Workers, Marines, Medics, Tanks, Fighters, Zerglings, Ultralisks
- **Multiple Building Types**: Command Centre, Barracks, Factory, Starport, Supply Depot, Refinery
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
- **Skirmish Mode**: Defend against enemy waves
- **Deathmatch**: All-out warfare

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
│   ├── main.cpp      # Main game logic
│   ├── game.js       # JavaScript game engine
│   ├── utils.js      # Utility functions
│   └── main.js       # Entry point
├── assets/
│   └── models/       # 3D models (.obj, .dae)
├── art/              # Sprites and textures
├── sounds/           # Audio files
├── starcraft_mod/    # Mod support
└── docs/             # Documentation
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
