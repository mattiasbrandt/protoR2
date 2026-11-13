#!/usr/bin/env bash
# Fetches the three open-licensed (OFL) fonts overlay.html draws the titles in,
# from the google/fonts repository, into ./fonts beside this script.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
base='https://github.com/google/fonts/raw/main/ofl'
curl -sfL -o "$here/fonts/SpaceGrotesk.ttf" "$base/spacegrotesk/SpaceGrotesk%5Bwght%5D.ttf"
curl -sfL -o "$here/fonts/Orbitron.ttf" "$base/orbitron/Orbitron%5Bwght%5D.ttf"
curl -sfL -o "$here/fonts/JetBrainsMono.ttf" "$base/jetbrainsmono/JetBrainsMono%5Bwght%5D.ttf"
ls -l "$here/fonts"
