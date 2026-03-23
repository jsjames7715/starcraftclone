# MiniStar – Extending the Mod

## New Units & Buildings (added to `units.lua`)
- **Refinery** – placeholder gas extractor.
- **Factory** – builds **Tank** heavy vehicle.
- **Tank** – armored artillery with a cannon.
- **Zergling** – fast melee unit for a “Zerg” flavor.

## Custom Graphics
Place PNG files (or .s3o models) in `starcraft_mod/art/` using the exact names referenced in `customParams.texture`:
```
command_center.png
worker.png
barracks.png
marine.png
supply_depot.png
refinery.png
factory.png
tank.png
zergling.png
```
If a file is missing the engine will generate a simple colored placeholder.

## Advanced ai (`advanced_ai.lua`)
- Builds a Barracks if none exists.
- Trains Marines until a small army is ready.
- Sends an attack order once 3 Marines are assembled.
- ai is referenced in `modinfo.lua` via `ai = {"advanced_ai"}`.

## Balancing
Adjust any of the following fields in `units.lua` to suit your play‑style:
- `buildcostmetal` – resource price.
- `maxdamage` – health.
- `maxVelocity` – movement speed.
- Weapon `damage`, `range`, `reloadtime`.
- `sightDistance` – vision radius.


## Multiplayer
1. Build the mod (`sdltool .` → `ministar.sdf`).
2. Start a host:
   ```bash
   spring -host -mod ministar.sdf
   ````
3. Other players join with:
   ```bash
   spring -join <host‑IP> -mod ministar.sdf
   ````
   (You may need to copy `ministar.sdf` to the same path on each client.)

## Next Steps
- Create a proper map with **SpringMap** and drop it in `maps/`.
- Replace placeholder `.s3o` models with real 3D assets (exported from Blender, etc.).
- Expand ai logic (resource scouting, expansion, tech‑tree).
- Polish ui (resource bars, minimap) by editing files under `gamedata/ui/`.

Happy RTS hacking!

## Asset Acquisition Guide
- **3D models** – Search CC0/Creative‑Commons assets on sites like **OpenGameArt.org**, **BlendSwap**, or **Sketchfab**. Look for low‑poly sci‑fi models (tanks, barracks, workers). Export them as **.obj** and run the Spring model exporter to get `.s3o` files.
- **Textures** – Use any image editor (Gimp, Krita, Photoshop) to create 256×256 PNGs. Name them exactly as listed in the `customParams.texture` table (e.g., `tank.png`).
- **Sounds** – Free‑to‑use SFX packs (e.g., **Freesound.org**, **OpenGameArt**) for construction, unit selection, attack, and engine noises. Place `.wav` files in `starcraft_mod/sounds/` and reference them in `gamedata/sounds/sounds.lua`.
- **Version control** – Keep your `art/`, `sounds/`, and `gamedata/` directories under Git so you can track asset updates.

## Full‑Feature Checklist (Road‑Map)
1. **Resource System** – Implement gas extraction, supply limits, and a ui resource bar that shows both minerals and gas.
2. **Tech‑Tree ui** – Add a research screen where players can spend resources to unlock new units/buildings.
3. **Advanced ai** – Write a modular ai manager that handles scouting, base expansion, and adaptive army composition.
4. **Multiplayer Sync** – Test with at least two clients; enable `Sync` debugging in `modinfo.lua` to catch desyncs early.
5. **Animations & Effects** – Use `.s3o` animation frames or particle effects (`gamedata/effects/`).
6. **Audio Mixer** – Add background music and per‑unit sound cues; expose volume sliders in the options menu.
7. **Polish ui** – Implement health bars (`unitHealth.lua`), tooltips (`tooltip.lua`), minimap zoom, and unit selection marquee.
8. **Balancing & Play‑testing** – Iterate on unit costs, build times, and ai aggression based on play‑tester feedback.
9. **Packaging** – Create a release script that zips `ministar.sdf`, `maps/`, `art/`, and `sounds/` for distribution.

When you finish each milestone, rebuild the mod with `sdltool .` and launch `spring ministar.sdf` to see the changes in action. Happy development!