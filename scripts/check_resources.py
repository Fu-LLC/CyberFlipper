#!/usr/bin/env python3
"""Audit resources.tar for anything that would cause Flipper updater error 13-168."""
import tarfile
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TAR = ROOT / "resources.tar"

t = tarfile.open(TAR)
members = t.getmembers()
files = [m for m in members if m.isfile()]

print(f"Total entries: {len(members)}, files: {len(files)}")
print()

# 1. PNG / image files (causes error 13 on Flipper updater)
images = [m for m in files if m.name.lower().endswith(('.png','.jpg','.jpeg','.gif','.webp','.bmp'))]
print(f"Image files ({len(images)}):")
for m in images:
    print(f"  {m.size:>8}  {m.name}")

# 2. Hidden / dot files
hidden = [m for m in files if m.name.split('/')[-1].startswith('.')]
print(f"\nHidden files ({len(hidden)}):")
for m in hidden:
    print(f"  {m.name}")

# 3. README / .md / .txt files that shouldn't be on device
docs = [m for m in files if m.name.lower().endswith(('.md', '.readme', '.txt')) and 'subghz' not in m.name.lower()]
print(f"\nDoc files NOT subghz ({len(docs)}):")
for m in docs[:20]:
    print(f"  {m.name}")

# 4. Files with FAT32-incompatible characters
bad_chars = [m for m in files if re.search(r'[<>:"|?*]', m.name)]
print(f"\nBad char filenames ({len(bad_chars)}):")
for m in bad_chars[:10]:
    print(f"  {m.name}")

# 5. Very long paths (FAT32 limit ~255 per component)
long_paths = [m for m in files if any(len(p) > 100 for p in m.name.split('/'))]
print(f"\nVery long path components ({len(long_paths)}):")
for m in long_paths[:10]:
    print(f"  {len(m.name):>4}  {m.name}")

# 6. Last 20 files processed (updater fails near end)
print(f"\nLast 20 files in tar order:")
for m in files[-20:]:
    print(f"  {m.size:>8}  {m.name}")

t.close()
