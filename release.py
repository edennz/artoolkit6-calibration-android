import xml.etree.ElementTree as ET
import os
tree = ET.parse('Source/camera-calibration/ARToolKit_CameraCalibration/src/main/res/values/strings.xml')
root = tree.getroot()
for value in root.findall('string'):
    print(value.get('name'))
    if  value.get('name') == "pref_calibrationServerDefault":
        value.text= str(os.getenv('ARTK_SERVER_URL_PROD','ARTK_URL'))
    if value.get('name') == 'pref_calibrationServerTokenDefault':
        value.text= str(os.getenv('ARTK_SERVER_TOKEN_PROD','ARTK_TOKEN'))

tree.write('Source/camera-calibration/ARToolKit_CameraCalibration/src/main/res/values/strings.xml')
