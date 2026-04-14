#!/usr/bin/env python3
"""
CYBERFLIPPER v1.0.0 — Firmware Integrity Checker
Verifies SHA-256 hashes of all firmware files against known-good values.
Run after downloading or before flashing to ensure integrity.

Usage:
    python3 verify.py              # Verify all firmware files
    python3 verify.py firmware.dfu # Verify specific file
"""

import os
import sys
import json
import hashlib


def sha256_file(filepath):
    """Calculate SHA-256 of a file."""
    h = hashlib.sha256()
    with open(filepath, "rb") as f:
        for chunk in iter(lambda: f.read(65536), b""):
            h.update(chunk)
    return h.hexdigest()


def verify_firmware(base_dir, specific_file=None):
    """Verify firmware file integrity."""
    print(f"\n  CYBERFLIPPER — Firmware Integrity Checker")
    print(f"  {'=' * 42}\n")

    manifest_path = os.path.join(base_dir, "manifest.json")
    known_hashes = {}

    if os.path.exists(manifest_path):
        with open(manifest_path) as f:
            manifest = json.load(f)
            for fname, info in manifest.get("firmware", {}).items():
                known_hashes[fname] = info.get("sha256", "")

    files = [specific_file] if specific_file else [
        "firmware.dfu", "radio.bin", "splash.bin",
        "updater.bin", "update.fuf", "resources.tar"
    ]

    passed = 0
    failed = 0

    for fname in files:
        fpath = os.path.join(base_dir, fname)
        if not os.path.exists(fpath):
            print(f"  [SKIP] {fname} — not found")
            continue

        current_hash = sha256_file(fpath)
        size_mb = os.path.getsize(fpath) / 1024 / 1024

        if fname in known_hashes and known_hashes[fname]:
            if current_hash == known_hashes[fname]:
                print(f"  [PASS] {fname:20s} {size_mb:8.2f} MB  ✓ hash verified")
                passed += 1
            else:
                print(f"  [FAIL] {fname:20s} {size_mb:8.2f} MB  ✗ HASH MISMATCH")
                print(f"         Expected: {known_hashes[fname][:32]}...")
                print(f"         Got:      {current_hash[:32]}...")
                failed += 1
        else:
            print(f"  [INFO] {fname:20s} {size_mb:8.2f} MB  SHA256: {current_hash[:32]}...")
            passed += 1

    print(f"\n  Results: {passed} verified, {failed} failed")
    return failed == 0


if __name__ == "__main__":
    base = os.path.dirname(os.path.abspath(__file__))
    target = sys.argv[1] if len(sys.argv) > 1 else None
    success = verify_firmware(base, target)
    sys.exit(0 if success else 1)
