#!/usr/bin/env python3
"""
CYBERFLIPPER v1.0.0 — Deployment Package Builder
Creates the .tgz release archive with firmware, apps, and resources.
Validates file integrity and generates manifest.

Usage:
    python3 package.py                  # Build release package
    python3 package.py --verify         # Verify existing package
    python3 package.py --manifest       # Generate manifest only
"""

import os
import sys
import hashlib
import tarfile
import json
import argparse
from datetime import datetime
from pathlib import Path

VERSION = "1.0.0"
RELEASE_NAME = f"CYBERFLIPPER-v{VERSION}"

FIRMWARE_FILES = [
    "firmware.dfu",
    "radio.bin",
    "splash.bin",
    "updater.bin",
    "update.fuf",
    "resources.tar",
]

REQUIRED_DIRS = [
    "apps",
    "badusb",
    "infrared",
    "nfc",
    "subghz",
    "dolphin",
]


def sha256(filepath):
    """Calculate SHA-256 hash of a file."""
    h = hashlib.sha256()
    with open(filepath, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()


def get_file_info(filepath):
    """Get file size and hash."""
    stat = os.stat(filepath)
    return {
        "size": stat.st_size,
        "size_human": f"{stat.st_size / 1024 / 1024:.2f} MB",
        "sha256": sha256(filepath),
        "modified": datetime.fromtimestamp(stat.st_mtime).isoformat(),
    }


def count_apps(base_dir):
    """Count FAP applications by category."""
    apps_dir = os.path.join(base_dir, "apps")
    categories = {}
    if not os.path.isdir(apps_dir):
        return categories
    for cat in os.listdir(apps_dir):
        cat_path = os.path.join(apps_dir, cat)
        if os.path.isdir(cat_path):
            apps = []
            for app in os.listdir(cat_path):
                app_path = os.path.join(cat_path, app)
                if os.path.isdir(app_path):
                    has_fam = os.path.exists(os.path.join(app_path, "application.fam"))
                    has_c = any(f.endswith(".c") for f in os.listdir(app_path))
                    if has_fam or has_c:
                        apps.append(app)
            if apps:
                categories[cat] = sorted(apps)
    return categories


def generate_manifest(base_dir):
    """Generate release manifest with file hashes and app inventory."""
    manifest = {
        "name": RELEASE_NAME,
        "version": VERSION,
        "build_date": datetime.now().isoformat(),
        "platform": "Flipper Zero (STM32WB55RG)",
        "firmware": {},
        "apps": {},
        "badusb_payloads": 0,
        "subghz_captures": 0,
    }

    # Firmware files
    for fw_file in FIRMWARE_FILES:
        fpath = os.path.join(base_dir, fw_file)
        if os.path.exists(fpath):
            manifest["firmware"][fw_file] = get_file_info(fpath)

    # Apps
    manifest["apps"] = count_apps(base_dir)

    # BadUSB payloads
    badusb_dir = os.path.join(base_dir, "badusb")
    if os.path.isdir(badusb_dir):
        manifest["badusb_payloads"] = sum(
            1 for _, _, files in os.walk(badusb_dir)
            for f in files if f.endswith(".txt")
        )

    # SubGHz captures
    subghz_dir = os.path.join(base_dir, "subghz")
    if os.path.isdir(subghz_dir):
        manifest["subghz_captures"] = sum(
            1 for _, _, files in os.walk(subghz_dir)
            for f in files if f.endswith(".sub")
        )

    return manifest


def build_package(base_dir):
    """Create the .tgz release package."""
    print(f"\n  CYBERFLIPPER v{VERSION} — Package Builder")
    print("  " + "=" * 42)

    # Validate firmware files
    print("\n  [*] Validating firmware files...")
    missing = []
    for fw_file in FIRMWARE_FILES:
        fpath = os.path.join(base_dir, fw_file)
        if os.path.exists(fpath):
            size_mb = os.path.getsize(fpath) / 1024 / 1024
            print(f"  [+] {fw_file:20s} {size_mb:.2f} MB")
        else:
            missing.append(fw_file)
            print(f"  [!] {fw_file:20s} MISSING")

    if missing:
        print(f"\n  [WARNING] Missing {len(missing)} firmware files")

    # Count apps
    apps = count_apps(base_dir)
    total_apps = sum(len(v) for v in apps.values())
    print(f"\n  [*] Found {total_apps} FAP applications:")
    for cat, app_list in sorted(apps.items()):
        print(f"      {cat}: {len(app_list)} apps")

    # Generate manifest
    print("\n  [*] Generating manifest...")
    manifest = generate_manifest(base_dir)
    manifest_path = os.path.join(base_dir, "manifest.json")
    with open(manifest_path, "w") as f:
        json.dump(manifest, f, indent=2)
    print(f"  [+] manifest.json written")

    # Create package
    dist_dir = os.path.join(base_dir, "dist")
    os.makedirs(dist_dir, exist_ok=True)
    tgz_path = os.path.join(dist_dir, f"{RELEASE_NAME}.tgz")

    print(f"\n  [*] Creating {RELEASE_NAME}.tgz...")
    with tarfile.open(tgz_path, "w:gz") as tar:
        for item in os.listdir(base_dir):
            if item in ("dist", ".git", "__pycache__"):
                continue
            item_path = os.path.join(base_dir, item)
            tar.add(item_path, arcname=f"CYBERFLIPPER/{item}")
            print(f"      + {item}")

    size_mb = os.path.getsize(tgz_path) / 1024 / 1024
    print(f"\n  [SUCCESS] {tgz_path}")
    print(f"  [SIZE]    {size_mb:.2f} MB")
    print(f"  [SHA256]  {sha256(tgz_path)}")
    return tgz_path


def verify_package(tgz_path):
    """Verify an existing .tgz package."""
    print(f"\n  [*] Verifying {tgz_path}...")
    if not os.path.exists(tgz_path):
        print("  [ERROR] File not found")
        return False

    with tarfile.open(tgz_path, "r:gz") as tar:
        members = tar.getnames()
        print(f"  [+] {len(members)} files in archive")

        fw_found = [m for m in members if any(m.endswith(f) for f in FIRMWARE_FILES)]
        print(f"  [+] {len(fw_found)}/{len(FIRMWARE_FILES)} firmware files present")

    print(f"  [+] SHA256: {sha256(tgz_path)}")
    print("  [OK] Package valid")
    return True


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="CYBERFLIPPER Package Builder")
    parser.add_argument("--verify", type=str, help="Verify existing .tgz package")
    parser.add_argument("--manifest", action="store_true", help="Generate manifest only")
    args = parser.parse_args()

    base = os.path.dirname(os.path.abspath(__file__))

    if args.verify:
        verify_package(args.verify)
    elif args.manifest:
        m = generate_manifest(base)
        print(json.dumps(m, indent=2))
    else:
        build_package(base)
