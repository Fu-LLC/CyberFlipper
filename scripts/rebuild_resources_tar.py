#!/usr/bin/env python3
"""Rebuild resources.tar with ONLY dolphin/ and u2f/ — no apps/.

The Flipper updater (error 13-168) fails when resources.tar contains
hundreds of FAP files. Core resources only go in resources.tar;
apps are installed separately via SD_CARD.zip.
"""
import tarfile
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "resources.tar"
SKIP_EXTS = {'.png', '.jpg', '.jpeg', '.gif', '.webp', '.bmp'}

# Only these directories belong in resources.tar
RESOURCE_DIRS = ["dolphin", "u2f"]

# Back up old tar
backup = ROOT / "resources.tar.bak"
if OUTPUT.exists():
    OUTPUT.rename(backup)
    print(f"Backed up old resources.tar -> resources.tar.bak")

with tarfile.open(OUTPUT, "w") as tar:
    count = 0
    for dir_name in RESOURCE_DIRS:
        dir_path = ROOT / dir_name
        if not dir_path.exists():
            print(f"  WARNING: {dir_name}/ not found, skipping")
            continue
        for file_path in sorted(dir_path.rglob("*")):
            if not file_path.is_file():
                continue
            if ".git" in file_path.parts:
                continue
            if file_path.suffix.lower() in SKIP_EXTS:
                print(f"  SKIP (image): {file_path.relative_to(ROOT)}")
                continue
            arcname = str(file_path.relative_to(ROOT)).replace("\\", "/")
            tar.add(file_path, arcname=arcname)
            count += 1

print(f"\nDone: {count} files in resources.tar")
print(f"Size: {OUTPUT.stat().st_size / 1024:.1f} KB")
print(f"\nContents:")
with tarfile.open(OUTPUT) as t:
    for m in t.getmembers():
        if m.isfile():
            print(f"  {m.size:>8}  {m.name}")
