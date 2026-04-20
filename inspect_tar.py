import struct
import tarfile, io

path_bak = r'C:/Users/P/Desktop/CyberFlipper/resources.tar.bak'
path_new = r'C:/Users/P/Desktop/CyberFlipper/resources.tar'

# Check original (backup)
try:
    with open(path_bak, 'rb') as f:
        bak = f.read(512)
        f.seek(0)
        full_bak = f.read()
    print('=== resources.tar.bak (original) ===')
    print('First 16 bytes hex:', bak[:16].hex())
    print('First 16 bytes repr:', repr(bak[:16]))
    print('Original size:', len(full_bak))
except Exception as e:
    print(f'Error reading bak: {e}')

# Check rebuilt
try:
    with open(path_new, 'rb') as f:
        new = f.read(512)
    print()
    print('=== resources.tar (rebuilt) ===')
    print('First 16 bytes hex:', new[:16].hex())
    print('First 16 bytes repr:', repr(new[:16]))
except Exception as e:
    print(f'Error reading new: {e}')

try:
    t = tarfile.open(fileobj=io.BytesIO(bak), mode='r:')
    print('Original: Valid plain tar')
except:
    print('Original: NOT a plain tar')

try:
    t2 = tarfile.open(fileobj=io.BytesIO(new), mode='r:')
    print('Rebuilt: Valid plain tar')
except:
    print('Rebuilt: NOT a plain tar')
