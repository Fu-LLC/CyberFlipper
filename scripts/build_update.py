#!/usr/bin/env python3
"""Build CyberFlipper SD card release package.

Produces:
  - CYBERFLIPPER-v<VER>-SD_CARD.zip

Installation: extract the zip and copy folders to the Flipper SD card
via the qFlipper SD Card tab. No firmware flashing required.
"""

import json
import sys
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED

ROOT = Path(__file__).resolve().parent.parent

SD_PATHS = [
    ROOT / "SD_CARD_READY",
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


# Directories packed into resources.tar (written to Flipper SD card)
RESOURCES_DIRS = ["subghz", "apps", "dolphin", "u2f"]

# File extensions that must NOT go on the Flipper (causes updater errors)
RESOURCES_EXCLUDE_EXT = {".png", ".jpg", ".jpeg", ".gif", ".webp"}





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
