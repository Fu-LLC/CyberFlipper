#!/usr/bin/env python3
"""Build CyberFlipper release packages in a cross-platform way.

Produces:
  - CYBERFLIPPER-v<VER>.tgz
  - CYBERFLIPPER-v<VER>-SD_CARD.zip
  - CYBERFLIPPER-v<VER>-FULL.zip

This script updates manifest.txt and update.fuf Info lines before packaging.
"""

import json
import sys
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import tarfile
import shutil

ROOT = Path(__file__).resolve().parent.parent
REQUIRED = [
    ROOT / "firmware.dfu",
    ROOT / "updater.bin",
    ROOT / "radio.bin",
    ROOT / "splash.bin",
    ROOT / "resources.tar",
    ROOT / "update.fuf",
    ROOT / "manifest.json",
]

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


def write_manifest_txt(version: str) -> None:
    content = """Filetype: Flipper Update Manifest
Version: 1
Info: CYBERFLIPPER v{version}
Target: 7
Loader: updater.bin
Firmware: firmware.dfu
Radio: radio.bin
Resources: resources.tar
Splashscreen: splash.bin
""".format(version=version)
    (ROOT / "manifest.txt").write_text(content, encoding="utf-8")


def patch_update_fuf(version: str) -> None:
    path = ROOT / "update.fuf"
    lines = path.read_text(encoding="utf-8").splitlines()
    updated = []
    for line in lines:
        if line.strip().startswith("Info:"):
            updated.append(f"Info: CYBERFLIPPER v{version}")
        else:
            updated.append(line)
    path.write_text("\n".join(updated) + "\n", encoding="utf-8")


# Directories packed into resources.tar (written to Flipper SD card)
RESOURCES_DIRS = ["subghz", "apps", "dolphin", "u2f"]

# File extensions that must NOT go on the Flipper (causes updater errors)
RESOURCES_EXCLUDE_EXT = {".png", ".jpg", ".jpeg", ".gif", ".webp"}


def build_resources_tar() -> Path:
    """Rebuild resources.tar from local directories, excluding non-Flipper files."""
    out = ROOT / "resources.tar"
    with tarfile.open(out, "w") as tar:
        for d in RESOURCES_DIRS:
            src = ROOT / d
            if not src.exists():
                continue
            for item in sorted(src.rglob("*")):
                if not item.is_file():
                    continue
                if item.suffix.lower() in RESOURCES_EXCLUDE_EXT:
                    continue
                if ".git" in item.parts:
                    continue
                arcname = item.relative_to(ROOT)
                tar.add(item, arcname=str(arcname))
    return out


def build_update_tgz(version: str) -> Path:
    build_dir = ROOT / "update" / f"CYBERFLIPPER-v{version}"
    if build_dir.exists():
        shutil.rmtree(build_dir)
    build_dir.mkdir(parents=True, exist_ok=True)

    for name in ["update.fuf", "firmware.dfu", "updater.bin", "radio.bin", "splash.bin", "resources.tar"]:
        shutil.copy2(ROOT / name, build_dir / name)

    tgz_name = ROOT / f"CYBERFLIPPER-v{version}.tgz"
    if tgz_name.exists():
        tgz_name.unlink()

    with tarfile.open(tgz_name, "w:gz") as tar:
        tar.add(build_dir, arcname="update")

    return tgz_name


def add_directory_to_zip(zipf: ZipFile, entry_root: Path, path: Path) -> None:
    if path.is_dir():
        for item in sorted(path.rglob("*")):
            if item.is_file():
                rel = item.relative_to(entry_root.parent)
                zipf.write(item, rel)


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


def build_full_zip(version: str, tgz_path: Path) -> Path:
    zip_name = ROOT / f"CYBERFLIPPER-v{version}-FULL.zip"
    if zip_name.exists():
        zip_name.unlink()
    with ZipFile(zip_name, "w", ZIP_DEFLATED) as zipf:
        zipf.write(tgz_path, tgz_path.name)
        for filename in ["manifest.json", "manifest.txt", "update.fuf"]:
            zipf.write(ROOT / filename, filename)
    return zip_name


def validate_required_files() -> None:
    missing = [str(path.name) for path in REQUIRED if not path.exists()]
    if missing:
        raise FileNotFoundError("Missing required files: " + ", ".join(missing))


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

    print(f"[*] Building CyberFlipper release for version: {version}")
    validate_required_files()
    write_manifest_txt(version)
    patch_update_fuf(version)
    build_resources_tar()

    tgz_path = build_update_tgz(version)
    sd_path = build_sd_zip(version)
    full_path = build_full_zip(version, tgz_path)

    print("[*] Local build complete. Files generated:")
    print(f"  - {tgz_path.name} ({format_size(tgz_path)})")
    print(f"  - {sd_path.name} ({format_size(sd_path)})")
    print(f"  - {full_path.name} ({format_size(full_path)})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
