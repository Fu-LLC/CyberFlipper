#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

VERSION="${1:-}"
if [[ -z "$VERSION" ]]; then
  if command -v python3 >/dev/null 2>&1; then
    VERSION=$(python3 - <<'PY'
import json
with open('manifest.json') as f:
    print(json.load(f)['version'])
PY
)
  else
    echo "ERROR: python3 is required to infer the version from manifest.json." >&2
    exit 1
  fi
fi

echo "[*] Building CyberFlipper release for version: $VERSION"

REQUIRED=(firmware.dfu updater.bin radio.bin splash.bin resources.tar update.fuf manifest.json)
MISSING=()
for f in "${REQUIRED[@]}"; do
  if [[ ! -f "$f" ]]; then
    MISSING+=("$f")
  fi
done
if [[ ${#MISSING[@]} -gt 0 ]]; then
  echo "ERROR: Missing required files:" >&2
  for f in "${MISSING[@]}"; do
    echo "  - $f" >&2
  done
  exit 1
fi

cat > manifest.txt <<EOF
Filetype: Flipper Update Manifest
Version: 1
Info: CYBERFLIPPER v${VERSION}
Target: 7
Loader: updater.bin
Firmware: firmware.dfu
Radio: radio.bin
Resources: resources.tar
Splashscreen: splash.bin
EOF

sed -i.bak "s/^Info:.*/Info: CYBERFLIPPER v${VERSION}/" update.fuf
rm -f update.fuf.bak

BUILD_DIR="update/CYBERFLIPPER-v${VERSION}"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cp update.fuf firmware.dfu updater.bin radio.bin splash.bin resources.tar "$BUILD_DIR/"

TGZ_NAME="CYBERFLIPPER-v${VERSION}.tgz"
rm -f "$TGZ_NAME"
tar -czvf "$TGZ_NAME" -C . "update/CYBERFLIPPER-v${VERSION}"

echo "[*] Created $TGZ_NAME"

touch "CYBERFLIPPER-v${VERSION}-SD_CARD.zip"
rm -f "CYBERFLIPPER-v${VERSION}-SD_CARD.zip"
zip -r "CYBERFLIPPER-v${VERSION}-SD_CARD.zip" SD_CARD_READY/ badusb/ infrared/ nfc/ subghz/ lfrfid/ dolphin/ u2f/ apps/ -x "*.git*"

echo "[*] Created CYBERFLIPPER-v${VERSION}-SD_CARD.zip"

FULL_ZIP="CYBERFLIPPER-v${VERSION}-FULL.zip"
rm -f "$FULL_ZIP"
zip -r "$FULL_ZIP" "$TGZ_NAME" manifest.json manifest.txt update.fuf

echo "[*] Created $FULL_ZIP"

echo "[*] Local build complete. Files generated:"
ls -lh "$TGZ_NAME" "CYBERFLIPPER-v${VERSION}-SD_CARD.zip" "$FULL_ZIP"
