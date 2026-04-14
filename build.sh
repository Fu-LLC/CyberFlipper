#!/bin/bash
# CYBERFLIPPER Build & Flash Toolchain
# Builds the firmware and creates the deployment package
# Usage: ./build.sh [clean|build|flash|package]

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
FW_DIR="${SCRIPT_DIR}"
BUILD_DIR="${FW_DIR}/build"
DIST_DIR="${FW_DIR}/dist"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
NC='\033[0m'

banner() {
    echo -e "${CYAN}"
    echo "  ██████╗██╗   ██╗██████╗ ███████╗██████╗ "
    echo " ██╔════╝╚██╗ ██╔╝██╔══██╗██╔════╝██╔══██╗"
    echo " ██║      ╚████╔╝ ██████╔╝█████╗  ██████╔╝"
    echo " ██║       ╚██╔╝  ██╔══██╗██╔══╝  ██╔══██╗"
    echo " ╚██████╗   ██║   ██████╔╝███████╗██║  ██║"
    echo "  ╚═════╝   ╚═╝   ╚═════╝ ╚══════╝╚═╝  ╚═╝"
    echo -e "          CYBERFLIPPER v1.0.0${NC}"
    echo ""
}

check_deps() {
    echo -e "${YELLOW}[*] Checking dependencies...${NC}"
    command -v python3 >/dev/null 2>&1 || { echo -e "${RED}python3 required${NC}"; exit 1; }
    command -v tar >/dev/null 2>&1 || { echo -e "${RED}tar required${NC}"; exit 1; }
    echo -e "${GREEN}[+] All dependencies satisfied${NC}"
}

clean() {
    echo -e "${YELLOW}[*] Cleaning build artifacts...${NC}"
    rm -rf "${BUILD_DIR}" "${DIST_DIR}"
    echo -e "${GREEN}[+] Clean complete${NC}"
}

package() {
    echo -e "${YELLOW}[*] Creating deployment package...${NC}"
    mkdir -p "${DIST_DIR}/CYBERFLIPPER"

    # Copy firmware files
    for f in firmware.dfu radio.bin splash.bin updater.bin update.fuf; do
        if [ -f "${FW_DIR}/${f}" ]; then
            cp "${FW_DIR}/${f}" "${DIST_DIR}/CYBERFLIPPER/"
            echo -e "  ${GREEN}[+] ${f}${NC}"
        fi
    done

    # Copy resources
    if [ -f "${FW_DIR}/resources.tar" ]; then
        cp "${FW_DIR}/resources.tar" "${DIST_DIR}/CYBERFLIPPER/"
        echo -e "  ${GREEN}[+] resources.tar${NC}"
    fi

    # Create the .tgz
    cd "${DIST_DIR}"
    tar -czf "CYBERFLIPPER-v1.0.0.tgz" CYBERFLIPPER/
    echo ""
    echo -e "${GREEN}[+] Package created: ${DIST_DIR}/CYBERFLIPPER-v1.0.0.tgz${NC}"
    echo -e "    Size: $(du -h CYBERFLIPPER-v1.0.0.tgz | cut -f1)"
}

flash_sd() {
    echo -e "${YELLOW}[*] Flash to SD card...${NC}"
    echo -e "  1. Insert your Flipper Zero's SD card"
    echo -e "  2. Copy CYBERFLIPPER/ folder to SD:\\update\\CYBERFLIPPER"
    echo -e "  3. Safely eject the SD card"
    echo -e "  4. On Flipper: Browse to update.fuf and run it"
    echo ""
    
    if [ -d "/Volumes" ]; then
        # macOS
        SD_CARDS=$(ls /Volumes/ 2>/dev/null | grep -i "flipper\|sd\|untitled" || true)
    elif [ -d "/media" ]; then
        # Linux
        SD_CARDS=$(ls /media/$USER/ 2>/dev/null || true)
    fi

    if [ -n "$SD_CARDS" ]; then
        echo -e "${CYAN}Detected removable drives:${NC}"
        echo "$SD_CARDS"
    fi
}

# Main
banner
case "${1:-package}" in
    clean)   check_deps; clean ;;
    build)   check_deps; echo -e "${YELLOW}[*] Building requires ./fbt toolchain${NC}" ;;
    flash)   flash_sd ;;
    package) check_deps; package ;;
    *)       echo "Usage: $0 {clean|build|flash|package}" ;;
esac
