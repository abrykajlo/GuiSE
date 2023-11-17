import os
import subprocess

CLANG_FORMAT = 'clang-format'
SOURCE_DIR = 'src'

def main():
    for (dirpath, _, filenames) in os.walk(SOURCE_DIR):
        for filename in filenames:
            if not is_src_file(filename):
                continue
            filepath = os.path.join(dirpath, filename)
            format_file(filepath)

def is_src_file(filename: str):
    return filename.endswith('.h') or filename.endswith('.cpp')

def format_file(filepath: str):
    subprocess.run([CLANG_FORMAT, '-i', filepath])

if __name__ == '__main__':
    main()