import tarfile, io

path_bak = r'C:/Users/P/Desktop/CyberFlipper/resources.tar.bak'

def test_tar(path):
    print(f'Testing {path}...')
    try:
        # Try as plain tar
        with open(path, 'rb') as f:
            t = tarfile.open(fileobj=f, mode='r:')
            print('  Valid plain tar')
            print('  Files:', t.getnames()[:5])
            return
    except Exception as e:
        print(f'  Not a plain tar: {e}')

    try:
        # Try as gzipped tar
        with open(path, 'rb') as f:
            t = tarfile.open(fileobj=f, mode='r:gz')
            print('  Valid gzipped tar')
            return
    except Exception as e:
        print(f'  Not a gzipped tar: {e}')

test_tar(path_bak)
