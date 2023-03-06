import xml.etree.ElementTree as ET
tree = ET.parse('CloudSeedXT.jucer')
root = tree.getroot()

with open('env.txt', 'w') as f:
    f.write(f"VERSION={root.attrib['version']}\n")
    f.write(f"PLUGIN_NAME={root.attrib['pluginName']}\n")
    f.write(f"PROJECT_NAME={root.attrib['name']}\n")
    f.write(f"COMPANY_NAME={root.attrib['companyName']}\n")
    f.write(f"COMPANY_WEBSITE={root.attrib['companyWebsite']}\n")
    f.write(f"VS_BUILD_DIR=./{root.find('EXPORTFORMATS').find('VS2022').attrib['targetFolder']}\n")
    f.write(f"{root.attrib['userNotes']}\n")


with open('env.txt', 'r') as f:
    while(True):
        line = f.readline()
        if not line:
            break
        print(f"SET {line.strip()}")