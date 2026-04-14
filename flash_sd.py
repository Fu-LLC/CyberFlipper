#!/usr/bin/env python3
"""
CYBERFLIPPER v1.0.0 — SD Card Flasher
Detects connected SD cards and copies the firmware package.

Usage:
    python3 flash_sd.py              # Auto-detect and flash
    python3 flash_sd.py -d E:        # Flash to specific drive
    python3 flash_sd.py --dry-run    # Preview without copying
"""

import os
import sys
import shutil
import platform
import argparse
from pathlib import Path


def detect_sd_cards():
    """Detect mounted SD cards / removable drives."""
    drives = []
    system = platform.system()

    if system == "Windows":
        import ctypes
        bitmask = ctypes.windll.kernel32.GetLogicalDrives()
        for letter in range(26):
            if bitmask & (1 << letter):
                drive = f"{chr(65 + letter)}:\\"
                try:
                    drive_type = ctypes.windll.kernel32.GetDriveTypeW(drive)
                    if drive_type == 2:  # DRIVE_REMOVABLE
                        drives.append(drive)
                except Exception:
                    pass
    elif system == "Darwin":  # macOS
        volumes = Path("/Volumes")
        if volumes.exists():
            drives = [str(v) for v in volumes.iterdir() if v.is_dir()]
    elif system == "Linux":
        media = Path(f"/media/{os.environ.get('USER', 'root')}")
        if media.exists():
            drives = [str(d) for d in media.iterdir() if d.is_dir()]
        mnt = Path("/mnt")
        if mnt.exists():
            drives.extend(str(d) for d in mnt.iterdir() if d.is_dir())

    return drives


def flash_to_drive(source_dir, target_drive, dry_run=False):
    """Copy CYBERFLIPPER firmware to SD card."""
    target = os.path.join(target_drive, "update", "CYBERFLIPPER")

    print(f"\n  CYBERFLIPPER SD Card Flasher")
    print(f"  {'=' * 35}")
    print(f"  Source: {source_dir}")
    print(f"  Target: {target}")
    print(f"  Mode:   {'DRY RUN' if dry_run else 'LIVE'}")
    print()

    # Files to copy
    files = [
        "firmware.dfu", "radio.bin", "splash.bin",
        "updater.bin", "update.fuf", "resources.tar"
    ]

    if not dry_run:
        os.makedirs(target, exist_ok=True)

    total_size = 0
    for f in files:
        src = os.path.join(source_dir, f)
        if os.path.exists(src):
            size = os.path.getsize(src)
            total_size += size
            size_str = f"{size / 1024 / 1024:.2f} MB"
            if dry_run:
                print(f"  [DRY] Would copy {f} ({size_str})")
            else:
                print(f"  [*] Copying {f} ({size_str})...", end="", flush=True)
                shutil.copy2(src, os.path.join(target, f))
                print(" OK")
        else:
            print(f"  [!] Skipping {f} (not found)")

    print(f"\n  Total: {total_size / 1024 / 1024:.2f} MB")
    if not dry_run:
        print(f"\n  [SUCCESS] Firmware written to {target}")
        print(f"  [NEXT] Safely eject SD card, insert into Flipper")
        print(f"  [NEXT] Navigate to update.fuf and run it")
    else:
        print(f"\n  [DRY RUN] No files were copied")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="CYBERFLIPPER SD Flasher")
    parser.add_argument("-d", "--drive", help="Target drive letter or mount point")
    parser.add_argument("--dry-run", action="store_true", help="Preview without copying")
    args = parser.parse_args()

    base = os.path.dirname(os.path.abspath(__file__))

    if args.drive:
        flash_to_drive(base, args.drive, args.dry_run)
    else:
        print("\n  [*] Detecting SD cards...")
        drives = detect_sd_cards()
        if drives:
            print(f"  [+] Found {len(drives)} removable drive(s):")
            for i, d in enumerate(drives):
                print(f"      [{i+1}] {d}")
            if not args.dry_run:
                choice = input("\n  Select drive number (or 'q' to quit): ").strip()
                if choice.isdigit() and 1 <= int(choice) <= len(drives):
                    flash_to_drive(base, drives[int(choice) - 1], args.dry_run)
                else:
                    print("  Cancelled.")
            else:
                flash_to_drive(base, drives[0], True)
        else:
            print("  [!] No removable drives detected")
            print("  [!] Use: python3 flash_sd.py -d E:\\")
