#!/usr/bin/env python3
"""Build CyberFlipper release packages.

Produces:
  - CYBERFLIPPER-v<VER>-UPDATE.zip  (firmware update — extract to SD:/update/)
  - CYBERFLIPPER-v<VER>-SD_CARD.zip (content pack — extract to SD:/ root)

Install order:
  1. Extract UPDATE.zip -> copy the CYBERFLIPPER-v<VER> folder to SD:/update/
  2. On Flipper: navigate to SD:/update/CYBERFLIPPER-v<VER>/update.fuf -> Run
  3. After reboot: copy SD_CARD.zip contents to SD:/ root via qFlipper SD Card tab
"""

import json
import shutil
import sys
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED

ROOT = Path(__file__).resolve().parent.parent

FIRMWARE_FILES = [
    "firmware.dfu",
    "radio.bin",
    "splash.bin",
    "updater.bin",
    "update.fuf",
]

SD_PATHS = [
    ROOT / "badusb",
    ROOT / "infrared",
    ROOT / "nfc",
    ROOT / "subghz",
    ROOT / "lfrfid",
    ROOT / "dolphin",
    ROOT / "u2f",
    ROOT / "apps",
]


def load_manifest_version() -> str:
    with open(ROOT / "manifest.json", "r", encoding="utf-8") as f:
        data = json.load(f)
    return str(data.get("version", "0.0.0"))


def build_update_zip(version: str) -> Path:
    """Build the firmware update zip: extract to SD:/update/ then run update.fuf."""
    zip_name = ROOT / f"CYBERFLIPPER-v{version}-UPDATE.zip"
    if zip_name.exists():
        zip_name.unlink()
    folder_name = f"CYBERFLIPPER-v{version}"
    missing = [f for f in FIRMWARE_FILES if not (ROOT / f).exists()]
    if missing:
        raise FileNotFoundError(f"Missing firmware files: {missing}")
    with ZipFile(zip_name, "w", ZIP_DEFLATED) as zipf:
        for fname in FIRMWARE_FILES:
            zipf.write(ROOT / fname, f"{folder_name}/{fname}")
    return zip_name


def build_sd_zip(version: str) -> Path:
    """Build the SD card content zip: extract to SD:/ root."""
    zip_name = ROOT / f"CYBERFLIPPER-v{version}-SD_CARD.zip"
    if zip_name.exists():
        zip_name.unlink()
    with ZipFile(zip_name, "w", ZIP_DEFLATED) as zipf:
        for path in SD_PATHS:
            if not path.exists():
                continue
            for item in sorted(path.rglob("*")):
                if not item.is_file() or ".git" in item.parts:
                    continue
                if item.suffix.lower() in {".png", ".jpg", ".jpeg", ".gif", ".webp"}:
                    continue
                rel = item.relative_to(ROOT)
                zipf.write(item, rel)
    return zip_name


def format_size(path: Path) -> str:
    size = path.stat().st_size
    for unit in ["B", "KB", "MB", "GB"]:
        if size < 1024.0:
            return f"{size:.1f}{unit}"
        size /= 1024.0
    return f"{size:.1f}TB"


def main(argv: list[str]) -> int:
    version = argv[1] if len(argv) > 1 else load_manifest_version()
    if not version:
        print("ERROR: Version is required.", file=sys.stderr)
        return 1

    print(f"[*] Building CyberFlipper v{version} release packages")
    update_path = build_update_zip(version)
    sd_path = build_sd_zip(version)
    print("[*] Build complete:")
    print(f"  - {update_path.name} ({format_size(update_path)})  <- firmware update")
    print(f"  - {sd_path.name} ({format_size(sd_path)})  <- SD card content")
    print()
    print("[*] Install order:")
    print(f"  1. Extract {update_path.name} -> copy CYBERFLIPPER-v{version}/ folder to SD:/update/")
    print(f"  2. On Flipper: navigate to SD:/update/CYBERFLIPPER-v{version}/update.fuf -> Run")
    print(f"  3. After reboot: extract {sd_path.name} and copy folders to SD:/ root via qFlipper SD Card tab")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
def build_sd_zip(version: str) -> Path:
    zip_name = ROOT / f"CYBERFLIPPER-v{version}-SD_CARD.zip"
    if zip_name.exists():
        zip_name.unlink()
    with ZipFile(zip_name, "w", ZIP_DEFLATED) as zipf:
        for path in SD_PATHS:
            if not path.exists():
                continue
            for item in sorted(path.rglob("*")):
                if item.is_file() and ".git" not in item.parts:
                    rel = item.relative_to(ROOT)
                    zipf.write(item, rel)
    return zip_name


def format_size(path: Path) -> str:
    size = path.stat().st_size
    for unit in ["B", "KB", "MB", "GB"]:
        if size < 1024.0:
            return f"{size:.1f}{unit}"
        size /= 1024.0
    return f"{size:.1f}TB"


def main(argv: list[str]) -> int:
    version = argv[1] if len(argv) > 1 else load_manifest_version()
    if not version:
        print("ERROR: Version is required.", file=sys.stderr)
        return 1

    print(f"[*] Building CyberFlipper SD card package for version: {version}")
    sd_path = build_sd_zip(version)
    print("[*] Build complete:")
    print(f"  - {sd_path.name} ({format_size(sd_path)})")
    print("[*] Extract and copy folders to Flipper SD card via qFlipper SD Card tab.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
