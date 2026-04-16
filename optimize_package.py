import os
import tarfile
import json
import hashlib

base_path = r"c:\Users\pfuru\OneDrive\Desktop\CYBERFLIPPER"
dist_path = os.path.join(base_path, "dist")
os.makedirs(dist_path, exist_ok=True)

# THE MASTERMIND OPTIMIZER: ONLY INCLUDE FLIPPER-READABLE FILES
ALLOWED_EXTENSIONS = {
    ".sub", ".nfc", ".rfid", ".ir", ".txt", ".fap", ".bm", ".png", ".plist", ".ibtn", ".u2f", ".fuf"
}

# DIRECTORIES REQUIRED BY FLIPPER SD CARD
FLIPPER_SD_ROOTS = [
    "subghz", "nfc", "lfrfid", "badusb", "infrared", "apps", "dolphin", "u2f", "ibutton"
]

def optimize_and_pack():
    print("[*] Starting Mastermind Optimization v1.2.1...")
    
    resource_tar_path = os.path.join(dist_path, "resources.tar")
    
    with tarfile.open(resource_tar_path, "w") as tar:
        for root_dir in FLIPPER_SD_ROOTS:
            src_dir = os.path.join(base_path, root_dir)
            if not os.path.exists(src_dir):
                continue
                
            print(f"  [+] Processing: {root_dir}")
            for root, dirs, files in os.walk(src_dir):
                for file in files:
                    ext = os.path.splitext(file)[1].lower()
                    if ext in ALLOWED_EXTENSIONS:
                        full_path = os.path.join(root, file)
                        # Relative path in tar (e.g. subghz/Vehicles/...)
                        rel_path = os.path.relpath(full_path, base_path)
                        tar.add(full_path, arcname=rel_path)

    # Calculate statistics
    tar_size = os.path.getsize(resource_tar_path) / (1024 * 1024)
    print(f"[SUCCESS] Optimized Resources: {tar_size:.2f} MB")

    # Final TGZ Build
    final_tgz = os.path.join(dist_path, "CYBERFLIPPER_v1.2.1_OPTIMIZED.tgz")
    print(f"[*] Creating {os.path.basename(final_tgz)}...")
    
    with tarfile.open(final_tgz, "w:gz") as tar:
        # Add the optimized resources
        tar.add(resource_tar_path, arcname="resources.tar")
        
        # Add firmware binaries if they exist
        binaries = ["firmware.dfu", "radio.bin", "update.fuf", "manifest.txt"]
        for b in binaries:
            b_path = os.path.join(base_path, b)
            if os.path.exists(b_path):
                tar.add(b_path, arcname=b)
            else:
                print(f"  [!] Missing critical binary: {b}")

    final_size = os.path.getsize(final_tgz) / (1024 * 1024)
    print(f"[COMPLETE] Final Optimized Size: {final_size:.2f} MB")
    print(f"[LOCATION] {final_tgz}")

if __name__ == "__main__":
    optimize_and_pack()
