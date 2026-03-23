#!/bin/bash
# convert_models.sh – converts placeholder OBJ files to Spring .s3o format.
# Requires the Spring model converter (s3oconv) which ships with the SDK.
# If s3oconv is not in PATH, adjust the variable below.

CONVERTER="s3oconv"
MODEL_DIR="assets/models"
OUTPUT_DIR="art"
mkdir -p "$OUTPUT_DIR"

# Enable nullglob so that if no matches, the loop runs zero times
shopt -s nullglob
for obj in "$MODEL_DIR"/*.obj; do
    name=$(basename "$obj" .obj)
    out="$OUTPUT_DIR/${name}.s3o"
    if command -v $CONVERTER >/dev/null 2>&1; then
        $CONVERTER "$obj" "$out"
        echo "Converted $obj → $out"
    else
        echo "s3oconv not found – copying OBJ as placeholder for $name"
        cp "$obj" "$OUTPUT_DIR/${name}.obj"
    fi
done
shopt -u nullglob
