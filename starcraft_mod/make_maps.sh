#!/bin/bash
# make_maps.sh – generate placeholder .smf map files for MiniStar
# These are empty files (1 KB) that the engine will reject as proper maps,
# but they serve as placeholders for the folder structure.
# Replace them with real maps created via SpringMap.

mkdir -p maps
for i in 1 2 3; do
    dd if=/dev/zero of=maps/map${i}.smf bs=1024 count=1 status=none
    echo "Created placeholder maps/map${i}.smf"
done
