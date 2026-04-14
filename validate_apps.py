#!/usr/bin/env python3
"""
CYBERFLIPPER v1.0.0 — App Validator
Validates all FAP application manifests and source files.
Checks for missing files, syntax issues, and build compatibility.

Usage:
    python3 validate_apps.py          # Validate all apps
    python3 validate_apps.py --fix    # Auto-fix common issues
"""

import os
import re
import sys
from pathlib import Path


REQUIRED_FIELDS = ["appid", "name", "apptype", "entry_point", "fap_category"]
APP_CATEGORIES = ["Tools", "Bluetooth", "subghz", "GPIO", "NFC", "RFID",
                   "Infrared", "Games", "Media", "Settings", "USB",
                   "Main", "Sub-GHz", "automotive", "passive", "iButton",
                   "badusb"]


def validate_fam(fam_path):
    """Validate an application.fam manifest."""
    issues = []
    with open(fam_path, "r") as f:
        content = f.read()

    # Check App() wrapper
    if "App(" not in content:
        issues.append("Missing App() wrapper")
        return issues

    # Check required fields
    for field in REQUIRED_FIELDS:
        if f'{field}=' not in content and f'{field} =' not in content:
            issues.append(f"Missing field: {field}")

    # Check entry_point matches
    entry_match = re.search(r'entry_point\s*=\s*"(\w+)"', content)
    if entry_match:
        entry = entry_match.group(1)
        # Check if the .c file has this function
        parent = os.path.dirname(fam_path)
        c_files = [f for f in os.listdir(parent) if f.endswith(".c")]
        if c_files:
            found = False
            for cf in c_files:
                with open(os.path.join(parent, cf), "r") as f:
                    if entry in f.read():
                        found = True
                        break
            if not found:
                issues.append(f"entry_point '{entry}' not found in .c files")

    return issues


def validate_c_source(c_path):
    """Validate a C source file for basic correctness."""
    issues = []
    with open(c_path, "r") as f:
        content = f.read()

    # Check required includes
    if "#include <furi.h>" not in content:
        issues.append("Missing #include <furi.h>")

    # Check for main entry function
    if "int32_t" not in content:
        issues.append("Missing int32_t entry point function")

    # Check for proper cleanup patterns
    if "view_port_alloc" in content and "view_port_free" not in content:
        issues.append("ViewPort allocated but not freed (memory leak)")

    if "furi_record_open" in content and "furi_record_close" not in content:
        issues.append("Record opened but not closed")

    if "malloc" in content and "free" not in content:
        issues.append("malloc() without free() (memory leak)")

    return issues


def scan_apps(base_dir):
    """Scan all apps directories and validate."""
    apps_dir = os.path.join(base_dir, "apps")
    if not os.path.isdir(apps_dir):
        print("  [ERROR] apps/ directory not found")
        return

    total = 0
    passed = 0
    failed = 0

    print(f"\n  CYBERFLIPPER App Validator")
    print(f"  {'=' * 35}\n")

    for category in sorted(os.listdir(apps_dir)):
        cat_path = os.path.join(apps_dir, category)
        if not os.path.isdir(cat_path):
            continue

        for app in sorted(os.listdir(cat_path)):
            app_path = os.path.join(cat_path, app)
            if not os.path.isdir(app_path):
                continue

            fam = os.path.join(app_path, "application.fam")
            c_files = [f for f in os.listdir(app_path) if f.endswith(".c")]

            if not os.path.exists(fam) and not c_files:
                continue

            total += 1
            all_issues = []

            if os.path.exists(fam):
                all_issues.extend(validate_fam(fam))

            for cf in c_files:
                all_issues.extend(validate_c_source(os.path.join(app_path, cf)))

            if all_issues:
                failed += 1
                print(f"  [WARN] {category}/{app}")
                for issue in all_issues:
                    print(f"         - {issue}")
            else:
                passed += 1
                print(f"  [OK]   {category}/{app}")

    print(f"\n  Results: {passed}/{total} passed, {failed} warnings")
    return failed == 0


if __name__ == "__main__":
    base = os.path.dirname(os.path.abspath(__file__))
    success = scan_apps(base)
    sys.exit(0 if success else 1)
