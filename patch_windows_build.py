import sys
import os

def main():
    sln_dir = os.environ["VS_BUILD_DIR"]
    files = os.listdir(sln_dir)
    for file in files:
        if not any(file.endswith(x) for x in [".sln", ".vcxproj", ".vcxproj.filters"]):
            continue

        filepath = os.path.join(sln_dir, file)
        with open(filepath, 'r') as f:
            content = f.read()

        content = content.replace(os.environ["JUCE_DIR"], "..\\..\\JUCE").replace(os.environ["JUCE_REL_DIR"], "..\\..\\JUCE")
        with open(filepath, 'w') as f:
            f.write(content)

if __name__ == '__main__':
    main()
