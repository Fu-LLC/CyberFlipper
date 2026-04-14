import os

def inject_scanlines(bm_path):
    """
    Injects high-tech scanlines into a raw .bm file.
    Wipes every 4th line to create a CRT/HUD effect.
    """
    if not os.path.exists(bm_path):
        return
    
    with open(bm_path, 'rb') as f:
        data = bytearray(f.read())
        
    # Standard Flipper BM is 128x64 = 1024 bytes.
    # 128 pixels = 16 bytes per row.
    if len(data) >= 1024:
        for row in range(0, 64, 4): # Every 4th row
            start = row * 16
            for col in range(16):
                # Force row to black (0x00)
                data[start + col] = 0x00
        
        with open(bm_path, 'wb') as f:
            f.write(data)
        return True
    return False

def revamp_all():
    print("  CYBERFLIPPER : MOVIE-QUALITY INJECTION ACTIVE")
    print("  " + "=" * 50)
    
    count = 0
    base_dir = "dolphin"
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith(".bm"):
                if inject_scanlines(os.path.join(root, file)):
                    count += 1
                    
    print(f"\n  [SUCCESS] Injected Cyber-Scanlines into {count} frames.")
    print("  [INFO] All legacy assets now feature the F-SERIES Neural HUD overlay.")

if __name__ == "__main__":
    revamp_all()
