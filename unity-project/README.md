# StarCraft Clone - Unity Project

A real StarCraft-style RTS game built with Unity, featuring proper graphics, UI, and game mechanics.

## Unity Version

Recommended: Unity 2022.3 LTS or newer

## Setup Instructions

### 1. Create New Unity Project

1. Open Unity Hub
2. Click "New Project"
3. Select "3D (Built-in Render Pipeline)" or "3D (URP)"
4. Name the project "StarCraftClone"
5. Click "Create Project"

### 2. Import Project Files

Copy the contents of `unity-project/` into your Unity project's `Assets/` folder:

```
YourUnityProject/
└── Assets/
    ├── Scripts/
    │   ├── GameManager.cs
    │   ├── Unit.cs
    │   ├── Building.cs
    │   ├── SelectionManager.cs
    │   ├── CameraController.cs
    │   ├── UIManager.cs
    │   ├── WaveSpawner.cs
    │   ├── WorkerAI.cs
    │   └── HealthBar.cs
    ├── Prefabs/
    ├── Scenes/
    ├── Materials/
    ├── Textures/
    ├── Sprites/
    └── UI/
```

### 3. Install TextMeshPro

1. Window > Package Manager
2. Search for "TextMeshPro"
3. Click "Install"

### 4. Set Up Scene

1. Create a new scene or open `MainScene.unity`
2. Add empty GameObjects and attach the scripts:
   - `GameManager` - Attach to empty GameObject named "GameManager"
   - `UIManager` - Attach to Canvas or empty "UIManager"
   - `SelectionManager` - Attach to empty "SelectionManager"
   - `CameraController` - Attach to Main Camera
   - `WaveSpawner` - Attach to empty "WaveSpawner"

### 5. Create Unit Prefabs

For each unit type, create a prefab with:
- 3D model or sprite
- Collider (Sphere or Box)
- Rigidbody (Is Kinematic = true)
- Unit script
- Health bar prefab

### 6. Create UI

Create UI Canvas with:
- Resource display (minerals, gas, supply)
- Selection panel
- Minimap
- Production queue
- Build panel
- Alert system
- Pause menu

## Project Structure

```
Assets/
├── Scripts/
│   ├── GameManager.cs      - Core game state, resources, time
│   ├── Unit.cs             - Unit base class with stats, movement, combat
│   ├── Building.cs         - Building class with production
│   ├── SelectionManager.cs - Unit/building selection system
│   ├── CameraController.cs - RTS camera with edge scroll
│   ├── UIManager.cs       - All UI elements
│   ├── WaveSpawner.cs      - Enemy wave spawning
│   ├── WorkerAI.cs         - Resource gathering logic
│   └── HealthBar.cs        - Health bar UI component
├── Prefabs/
│   ├── Units/
│   │   ├── Marine.prefab
│   │   ├── Tank.prefab
│   │   └── ...
│   ├── Buildings/
│   │   ├── CommandCenter.prefab
│   │   ├── Barracks.prefab
│   │   └── ...
│   └── Effects/
│       ├── AttackEffect.prefab
│       └── DeathEffect.prefab
├── Scenes/
│   └── MainScene.unity
├── Materials/
├── Textures/
├── Sprites/
│   ├── UI/
│   └── Icons/
└── UI/
    └── Prefabs/
```

## Key Features

### Units
- 40+ unit types across Terran, Zerg, and Protoss
- Individual stats: HP, damage, armor, range, speed
- Attack, move, patrol, hold position commands
- Special abilities

### Buildings
- Production facilities for each unit type
- Resource processing (refineries, extractors)
- Defensive structures (turrets, cannons)
- Tech tree dependencies

### Combat
- Real-time combat with attack cooldowns
- Armor and damage calculation
- Attack ranges and line of sight
- Target prioritization

### Economy
- Resource gathering with workers
- Supply management
- Production queues
- Tech upgrades

### AI
- Enemy wave spawning
- Wave scaling with difficulty
- Elite and boss enemies
- Resource management

## Controls

| Action | Control |
|--------|---------|
| Move Camera | WASD / Arrow Keys / Edge Scroll |
| Rotate Camera | Q/E or Middle Mouse |
| Zoom | R/F or Scroll Wheel |
| Select Unit | Left Click |
| Select Multiple | Left Click + Drag |
| Add to Selection | Shift + Click |
| Command Unit | Right Click |
| Attack-Move | A + Right Click |
| Hold Position | H |
| Stop | S |
| Patrol | P |
| Pause | Escape |

## Script Hooks

### Creating a Unit
```csharp
Unit marine = Unit.CreateUnit(UnitType.Marine, spawnPosition, Team.Player);
```

### Getting Unit Data
```csharp
UnitData marineData = UnitDatabase.GetUnitData(UnitType.Marine);
int cost = marineData.mineralCost;
```

### Training a Unit
```csharp
Building barracks = GetSelectedBarracks();
barracks.QueueUnit(UnitType.Marine);
```

## Asset Requirements

### Models
- Unit models (can be simple 3D primitives for prototyping)
- Building models
- Terrain tiles

### Sprites (2D mode)
- Unit sprites for each direction
- Building sprites
- UI elements

### Textures
- Terrain textures
- Unit/building textures
- UI skin textures

### Audio
- Unit voice lines
- Combat sounds
- UI sounds
- Music

## Next Steps

1. Create 3D models or import sprites
2. Set up proper materials and shaders
3. Configure unit prefabs with correct stats
4. Build UI panels
5. Add audio
6. Playtest and balance

## License

MIT License
