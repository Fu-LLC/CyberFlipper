import os
from PIL import Image

def png_to_bm(png_path, out_path):
    """
    Converts a PNG image to a Flipper Zero compatible 1-bit raw bitmap (.bm).
    The output is exactly 1024 bytes (128x64 / 8).
    Bits are packed LSB-first.
    """
    if not os.path.exists(png_path):
        # Fallback to empty if not found, but we will try to find our generated ones
        return False
        
    try:
        img = Image.open(png_path).convert('1')
        img = img.resize((128, 64))
        pixels = list(img.getdata())
        
        bm_data = bytearray()
        for i in range(0, len(pixels), 8):
            byte = 0
            for bit in range(8):
                if i + bit < len(pixels):
                    # Flipper display driver (ST7567) uses LSB-first packing for rows
                    # White (255) is 1, Black (0) is 0
                    if pixels[i + bit] > 0:
                        byte |= (1 << bit)
            bm_data.append(byte)
            
        with open(out_path, 'wb') as f:
            f.write(bm_data)
        print(f"  [+] Converted: {os.path.basename(png_path)} -> {os.path.basename(out_path)}")
        return True
    except Exception as e:
        print(f"  [!] Error converting {png_path}: {e}")
        return False

def build_anims():
    print("\n  CYBERFLIPPER F-SERIES : ANIMATION COMPILER v1.0")
    print("  " + "=" * 50)
    
    # Mapping our best generated frames to themes
    mapping = {
        "F_BOOT_128x64": ["f_boot_0v2_1776185500787.png", "f_boot_2_1776185482160.png"],
        "F_PULSE_128x64": ["f_pulse_1_final_1776185640388.png"],
        "F_GLITCH_128x64": ["f_glitch_0_final_1776185652205.png"]
    }
    
    # Locate where Gemini stores session images (absolute path provided in metadata)
    # We will assume they are in the current appData brain dir
    brain_dir = r"C:\Users\pfuru\.gemini\antigravity\brain\b709f490-e03a-4b6a-b65b-2c403f66cdea"
    
    for anim_name, frames in mapping.items():
        out_dir = os.path.join("dolphin", anim_name)
        os.makedirs(out_dir, exist_ok=True)
        
        for idx, frame_fname in enumerate(frames):
            png_path = os.path.join(brain_dir, frame_fname)
            out_path = os.path.join(out_dir, f"frame_{idx}.bm")
            png_to_bm(png_path, out_path)
            
    print("\n  [SUCCESS] All production .bm files generated.")

if __name__ == "__main__":
    build_anims()
