# CYBERFLIPPER v1.0.0 — Makefile
# Cross-platform build system for FAP apps and release packaging
#
# Usage:
#   make package     — Build .tgz release
#   make validate    — Validate all FAP apps
#   make flash       — Flash to SD card
#   make clean       — Remove build artifacts
#   make inventory   — Show full app inventory
#   make checksum    — Generate checksums for all files

SHELL := /bin/bash
VERSION := 1.0.0
RELEASE := CYBERFLIPPER-v$(VERSION)
DIST_DIR := dist
BUILD_DIR := build

# Firmware files
FW_FILES := firmware.dfu radio.bin splash.bin updater.bin update.fuf resources.tar

# App directories
TOOLS_DIR := apps/Tools
BT_DIR := apps/Bluetooth
SUBGHZ_DIR := apps/subghz
GPIO_DIR := apps/GPIO

.PHONY: all package validate flash clean inventory checksum help

all: validate package

help:
	@echo ""
	@echo "  CYBERFLIPPER v$(VERSION) Build System"
	@echo "  ====================================="
	@echo ""
	@echo "  make package    — Build .tgz release archive"
	@echo "  make validate   — Validate all FAP applications"
	@echo "  make flash      — Flash firmware to SD card"
	@echo "  make clean      — Remove build artifacts"
	@echo "  make inventory  — Display full app inventory"
	@echo "  make checksum   — Generate SHA-256 checksums"
	@echo ""

package:
	@echo "[*] Building $(RELEASE).tgz..."
	@python3 package.py
	@echo "[+] Done."

validate:
	@echo "[*] Validating FAP applications..."
	@python3 validate_apps.py

flash:
	@echo "[*] SD Card Flash Tool..."
	@python3 flash_sd.py

clean:
	@echo "[*] Cleaning..."
	@rm -rf $(DIST_DIR) $(BUILD_DIR) manifest.json
	@echo "[+] Clean."

inventory:
	@echo ""
	@echo "  === CYBERFLIPPER v$(VERSION) APP INVENTORY ==="
	@echo ""
	@echo "  Tools:"; @ls -1 $(TOOLS_DIR) 2>/dev/null | sed 's/^/    /'
	@echo "  Bluetooth:"; @ls -1 $(BT_DIR) 2>/dev/null | sed 's/^/    /'
	@echo "  SubGHz:"; @ls -1 $(SUBGHZ_DIR) 2>/dev/null | sed 's/^/    /'
	@echo "  GPIO:"; @ls -1 $(GPIO_DIR) 2>/dev/null | sed 's/^/    /'
	@echo ""

checksum:
	@echo "[*] Generating checksums..."
	@for f in $(FW_FILES); do \
		if [ -f $$f ]; then \
			echo "  $$(sha256sum $$f)"; \
		fi; \
	done
	@echo "[+] Done."
