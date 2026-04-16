import os
import shutil

src_root = r"c:\Users\pfuru\OneDrive\Desktop\CYBERFLIPPER"
dst_root = r"c:\Users\pfuru\OneDrive\Desktop\CyberFlipper_SD"

# FLIPPER-READABLE EXTENSIONS ONLY (The "Tactical Essence")
ALLOWED_EXTENSIONS = {
    ".sub", ".nfc", ".rfid", ".ir", ".txt", ".fap", ".bm", ".png", ".plist", ".ibtn", ".u2f", ".fuf"
}

# MANDATORY FLIPPER SD CARD DIRECTORIES
TACTICAL_DIRS = [
    "subghz", "nfc", "lfrfid", "badusb", "infrared", "apps", "dolphin", "u2f", "ibutton"
]

def build_refined_distribution():
    print("[*] Building Refined CyberFlipper Distribution...")
    
    if os.path.exists(dst_root):
        shutil.rmtree(dst_root)
    os.makedirs(dst_root)
    
    # 1. Copy mandatory SD directory structure
    for d in TACTICAL_DIRS:
        src_path = os.path.join(src_root, d)
        if not os.path.exists(src_path):
            continue
            
        dst_path = os.path.join(dst_root, d)
        os.makedirs(dst_path)
        
        print(f"  [+] Distilling Tactical Assets: {d}")
        for root, subdirs, files in os.walk(src_path):
            for file in files:
                ext = os.path.splitext(file)[1].lower()
                if ext in ALLOWED_EXTENSIONS:
                    # Construct relative path to maintain structure
                    rel_path = os.path.relpath(root, src_path)
                    target_dir = os.path.join(dst_path, rel_path)
                    os.makedirs(target_dir, exist_ok=True)
                    
                    shutil.copy2(os.path.join(root, file), os.path.join(target_dir, file))

    # 2. Add finalized firmware binaries to the root
    binaries = ["firmware.dfu", "radio.bin", "update.fuf", "manifest.txt"]
    for b in binaries:
        b_src = os.path.join(src_root, b)
        if os.path.exists(b_src):
            shutil.copy2(b_src, os.path.join(dst_root, b))
            print(f"  [+] Unified Firmware Binary: {b}")

    # 3. Add refined README for the hardware users
    readme_src = os.path.join(src_root, "README.md")
    if os.path.exists(readme_src):
        shutil.copy2(readme_src, os.path.join(dst_root, "README.md"))

    print(f"\n[SUCCESS] Refined CyberFlipper Deployment Ready!")
    print(f"[LOCATION] {dst_root}")
    print("[STATS] All development scripts, bulk data, and git history removed.")

if __name__ == "__main__":
    build_refined_distribution()
