import sys
import os

def main():
        filepath = os.path.join("Packaging", "Packaging.pkgproj.template")
        with open(filepath, 'r') as f:
            content = f.read()

        content = content.replace("PROJECT_NAME.component", f"{os.environ['PROJECT_NAME']}.component")
        content = content.replace("PROJECT_NAMEAU", f"{os.environ['PROJECT_NAME']}AU")
        content = content.replace("PROJECT_NAME.vst3", f"{os.environ['PROJECT_NAME']}.vst3")
        content = content.replace("PROJECT_NAMEVST3", f"{os.environ['PROJECT_NAME']}VST3")
        content = content.replace("PLUGIN_NAME", f"{os.environ['PLUGIN_NAME']}")
        content = content.replace("PLUGIN_VERSION", f"{os.environ['VERSION']}")
        content = content.replace("COMPANY_NAME", f"{os.environ['COMPANY_NAME'].lower().replace(' ', '')}")
        content = content.replace("/Users/USERNAME/Library", f"{os.environ['HOME']}/Library")
        
        with open(filepath.replace(".template", ""), 'w') as f:
            f.write(content)

if __name__ == '__main__':
    main()
