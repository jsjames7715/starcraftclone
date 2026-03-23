#!/bin/bash
# generate_sounds.sh – creates tiny silent .wav placeholders for MiniStar.
# The same 0.1 s silent wav is reused for all cues.
BASE64_WAV="UklGRiQAAABXQVZFZm10IBAAAAABAAEAIlYAAESsAAACABAAZGF0YQAAAAA="
mkdir -p ../sounds
for name in construct train attack select; do
    out="../sounds/${name}.wav"
    echo "$BASE64_WAV" | base64 -d > "$out"
    echo "Created $out"
done
