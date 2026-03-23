# MiniStar - Build and Run Instructions

## Prerequisites
- Linux distribution (Ubuntu/Debian recommended)
- Spring RTS engine installed (including development tools for sdltool)
  - On Ubuntu/Debian: `sudo apt-get install spring spring-dev`
  - On Arch: `sudo pacman -S spring`
  - On other distros, check your package manager or build from source: https://springrts.com/wiki/CompileFromSource

## Building the Mod
1. Clone or copy this repository to your machine.
2. Navigate to the project root:
   ```bash
   cd /path/to/MiniStar
   ```
3. Generate placeholder assets (optional, but recommended for first run):
   ```bash
   ./generate_textures.sh   # creates placeholder PNGs in art/
   ./convert_models.sh      # copies OBJ files as placeholders to art/ (since s3oconv may not be installed)
   ```
   Note: If you have the Spring SDK installed, `s3oconv` will convert OBJ to proper .s3o files.
4. Build the .sdf archive:
   ```bash
   sdltool .   # creates ministar.sdf in the current directory
   ```
   If you get an error about `sdltool not found`, ensure the Spring development package is installed.

## Running the Game
```bash
spring ministar.sdf
```
This will launch the game with the default map (map1.smf, map2.smf, or map3.smf from the maps/ folder).

## Hosting a LAN Game
1. On the host machine:
   ```bash
   spring -host -mod ministar.sdf
   ```
2. On client machines (same network):
   ```bash
   spring -join <host-IP> -mod ministar.sdf
   ```

## Replacing Placeholders with Polished Assets
- **3D Models**: Replace files in `assets/models/` with your own .obj or .fbx files (keep the same base names). Then re-run `./convert_models.sh` (if you have s3oconv) or manually convert them to .s3o and place them in `art/`.
- **Textures**: Replace the PNG files in `art/` with your own artwork (same names, e.g., `tank.png`). Recommended size: 256x256 or larger.
- **Sounds**: Replace the WAV files in `sounds/` with your own sound effects (e.g., `construct.wav`, `train.wav`).
- **Effects**: Replace `gamedata/effects/explosion.s3e` with a real effect created via the Spring SFXEditor.

## Development Tips
- Re-run `sdltool .` after any change to `units.lua`, `scripts/`, `ai/`, or `gamedata/` to rebuild the mod.
- Use the in-game console (`~`) to debug: `Spring.Echo(Spring.GetTeamResources(Spring.GetMyTeamID(), "metal"))`.
- Adjust unit stats in `gamedata/unitdefs/units.lua` to balance gameplay.
- Expand the AI in `gamedata/ai/advanced_ai.lua` for more sophisticated behavior.
- Add new units/buildings by following the existing patterns in `units.lua` and adding corresponding scripts.

## Troubleshooting
- **sdltool not found**: Install the Spring development package (see Prerequisites).
- **Missing models/textures**: The engine will log warnings to the console and show placeholder cubes/colors.
- **Out of sync in multiplayer**: Ensure all clients are using the exact same .sdf file. Disable any cheats or speed mods.

Enjoy building and battling in MiniStar!