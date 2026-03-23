# generate_textures.sh – creates simple placeholder PNGs for each unit/building texture
# Uses a 1x1 transparent PNG base64 blob and stretches it to 64×64.
# Requires ImageMagick's "convert" (apt install imagemagick). If not available, the raw PNG will be saved.

BASE64_DATA="iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMB/6X5WzAAAAAASUVORK5CYII="

TEXTURES=(
    command_center
    worker
    barracks
    marine
    supply_depot
    refinery
    factory
    tank
    zergling
    fighter
    medic
    siege_tank
)

mkdir -p ../art

for name in "${TEXTURES[@]}"; do
    OUT="../art/${name}.png"
    echo "${BASE64_DATA}" | base64 -d > "$OUT"
    # If convert is available, resize to 64x64 for a visible placeholder
    if command -v convert >/dev/null 2>&1; then
        convert "$OUT" -resize 64x64 "$OUT"
    fi
    echo "Created $OUT"
done
