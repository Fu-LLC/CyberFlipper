import os
import math

def save_bm(data, path):
    with open(path, 'wb') as f:
        f.write(data)

def pack_bits(pixels):
    """Packs 128x64 bit list into 1024 bytes (LSB first)."""
    data = bytearray()
    for i in range(0, len(pixels), 8):
        byte = 0
        for bit in range(8):
            if i + bit < len(pixels) and pixels[i + bit]:
                byte |= (1 << bit)
        data.append(byte)
    return data

def generate_radar_frames(folder):
    os.makedirs(folder, exist_ok=True)
    # Also create in 'external' for system-wide priority
    ext_folder = folder.replace("dolphin/", "dolphin/external/")
    os.makedirs(ext_folder, exist_ok=True)
    for frame in range(4):
        pixels = [0] * (128 * 64)
        radius = (frame + 1) * 15
        for y in range(64):
            for x in range(128):
                dx, dy = x - 64, y - 32
                dist = math.sqrt(dx*dx + dy*dy)
                if abs(dist - radius) < 2:
                    pixels[y * 128 + x] = 1
        data = pack_bits(pixels)
        save_bm(data, os.path.join(folder, f"frame_{frame}.bm"))
        save_bm(data, os.path.join(ext_folder, f"frame_{frame}.bm"))

def generate_grid_frames(folder):
    os.makedirs(folder, exist_ok=True)
    # Also create in 'internal' for system-wide priority
    int_folder = folder.replace("dolphin/", "dolphin/internal/")
    os.makedirs(int_folder, exist_ok=True)
    for frame in range(4):
        pixels = [0] * (128 * 64)
        offset = frame * 4
        for y in range(64):
            for x in range(128):
                if (x + offset) % 16 == 0 or (y + offset) % 16 == 0:
                    pixels[y * 128 + x] = 1
        data = pack_bits(pixels)
        save_bm(data, os.path.join(folder, f"frame_{frame}.bm"))
        save_bm(data, os.path.join(int_folder, f"frame_{frame}.bm"))

def generate_noise_frames(folder):
    os.makedirs(folder, exist_ok=True)
    int_folder = folder.replace("dolphin/", "dolphin/internal/")
    os.makedirs(int_folder, exist_ok=True)
    import random
    for frame in range(4):
        pixels = [random.choice([0, 1]) for _ in range(128 * 64)]
        data = pack_bits(pixels)
        save_bm(data, os.path.join(folder, f"frame_{frame}.bm"))
        save_bm(data, os.path.join(int_folder, f"frame_{frame}.bm"))

def generate_wave_frames(folder):
    os.makedirs(folder, exist_ok=True)
    ext_folder = folder.replace("dolphin/", "dolphin/external/")
    os.makedirs(ext_folder, exist_ok=True)
    for frame in range(4):
        pixels = [0] * (128 * 64)
        offset = frame * (math.pi / 2)
        for x in range(128):
            y = int(32 + 20 * math.sin(x/10 + offset))
            if 0 <= y < 64:
                pixels[y * 128 + x] = 1
        data = pack_bits(pixels)
        save_bm(data, os.path.join(folder, f"frame_{frame}.bm"))
        save_bm(data, os.path.join(ext_folder, f"frame_{frame}.bm"))

def main():
    print("  CYBERFLIPPER : NEURAL ASSET GENERATOR v1.0")
    print("  " + "=" * 50)
    
    # Theme 1: Tactical Radar
    print("  [*] Generating F_RADAR_PRO...")
    generate_radar_frames("dolphin/F_RADAR_PRO")
    
    # Theme 2: Matrix Grid
    print("  [*] Generating F_GRID_PRO...")
    generate_grid_frames("dolphin/F_GRID_PRO")
    
    # Theme 3: Signal Wave
    print("  [*] Generating F_WAVE_PRO...")
    generate_wave_frames("dolphin/F_WAVE_PRO")
    
    # Theme 4: High-Entropy Noise
    print("  [*] Generating F_NOISE_PRO...")
    generate_noise_frames("dolphin/F_NOISE_PRO")
    
    # Update manifest
    with open("dolphin/manifest.txt", "w") as f:
        f.write("Filetype: Flipper Animation Manifest\nVersion: 1\n")
        
        # External Animations
        for theme in ["external/F_RADAR_PRO", "external/F_WAVE_PRO"]:
            f.write(f"\nName: {theme}\n")
            f.write("Min butthurt: 0\nMax butthurt: 14\nMin level: 1\nMax level: 30\nWeight: 100\n")
            
        # Internal Animations
        for theme in ["internal/F_GRID_PRO", "internal/F_NOISE_PRO"]:
            f.write(f"\nName: {theme}\n")
            f.write("Min butthurt: 0\nMax butthurt: 14\nMin level: 1\nMax level: 30\nWeight: 100\n")
        
    print("\n  [SUCCESS] Professional Mathematical Assets Generated.")

if __name__ == "__main__":
    main()
