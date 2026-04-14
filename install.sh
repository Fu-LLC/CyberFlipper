#!/bin/bash
# CYBERFLIPPER v1.0.0 — Quick Install Script
# Downloads and installs the latest CYBERFLIPPER release to SD card
#
# Usage:
#   curl -sL https://raw.githubusercontent.com/FurulieLLC/CyberFlipper/main/install.sh | bash
#   ./install.sh /path/to/sd/card

set -e

VERSION="1.0.0"
REPO="FurulieLLC/CyberFlipper"
RELEASE_URL="https://github.com/${REPO}/releases/download/v${VERSION}/CYBERFLIPPER-v${VERSION}.tgz"

RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${CYAN}"
echo "  ╔══════════════════════════════════════╗"
echo "  ║   CYBERFLIPPER v${VERSION} Installer     ║"
echo "  ╚══════════════════════════════════════╝"
echo -e "${NC}"

# Detect SD card
SD_PATH="${1:-}"
if [ -z "$SD_PATH" ]; then
    echo -e "${YELLOW}[*] Auto-detecting SD card...${NC}"
    if [ -d "/Volumes" ]; then
        # macOS
        for vol in /Volumes/*/; do
            if [ -d "$vol" ] && [ "$vol" != "/Volumes/Macintosh HD/" ]; then
                SD_PATH="$vol"
                break
            fi
        done
    elif [ -d "/media/$USER" ]; then
        # Linux
        for mnt in /media/$USER/*/; do
            if [ -d "$mnt" ]; then
                SD_PATH="$mnt"
                break
            fi
        done
    fi
fi

if [ -z "$SD_PATH" ]; then
    echo -e "${RED}[!] No SD card detected. Specify path:${NC}"
    echo "    ./install.sh /path/to/sdcard"
    exit 1
fi

echo -e "${GREEN}[+] Target: ${SD_PATH}${NC}"

# Download
TMPDIR=$(mktemp -d)
TGZ="${TMPDIR}/CYBERFLIPPER-v${VERSION}.tgz"

echo -e "${YELLOW}[*] Downloading CYBERFLIPPER v${VERSION}...${NC}"
if command -v curl &>/dev/null; then
    curl -L -o "$TGZ" "$RELEASE_URL"
elif command -v wget &>/dev/null; then
    wget -O "$TGZ" "$RELEASE_URL"
else
    echo -e "${RED}[!] curl or wget required${NC}"
    exit 1
fi

echo -e "${YELLOW}[*] Extracting...${NC}"
mkdir -p "${SD_PATH}/update"
tar -xzf "$TGZ" -C "${SD_PATH}/update/"

echo -e "${YELLOW}[*] Verifying...${NC}"
INSTALLED="${SD_PATH}/update/CYBERFLIPPER"
for f in firmware.dfu radio.bin updater.bin update.fuf; do
    if [ -f "${INSTALLED}/${f}" ]; then
        echo -e "  ${GREEN}[✓] ${f}${NC}"
    else
        echo -e "  ${RED}[✗] ${f} missing${NC}"
    fi
done

# Cleanup
rm -rf "$TMPDIR"

echo ""
echo -e "${GREEN}╔══════════════════════════════════════╗${NC}"
echo -e "${GREEN}║   INSTALLATION COMPLETE              ║${NC}"
echo -e "${GREEN}╚══════════════════════════════════════╝${NC}"
echo ""
echo "  Next steps:"
echo "  1. Safely eject the SD card"
echo "  2. Insert into your Flipper Zero"
echo "  3. Navigate to: update/CYBERFLIPPER/update.fuf"
echo "  4. Run the update"
echo ""
