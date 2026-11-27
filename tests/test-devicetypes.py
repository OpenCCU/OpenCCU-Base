#!/usr/bin/env python3
"""Device descriptions in src/devicetypes and their WebUI device database entries."""
from pathlib import Path
import re
import unittest
import xml.etree.ElementTree as ET

SOURCE = Path(__file__).resolve().parents[1]
DEVICETYPES = SOURCE / "src/devicetypes"
DEVDESCR = SOURCE / "src/webui/www_source/config/devdescr"
WWW = SOURCE / "src/webui/www"

DECLARATION = re.compile(rb'^<\?xml[^>]*encoding="([^"]+)"', re.IGNORECASE)
IMAGE = re.compile(r'lappend\s+PATHLIST\s+\[list\s+\d+\s+"([^"]+)"\]')


def device_files():
    for family in ("rftypes", "hs485types"):
        yield from sorted((DEVICETYPES / family).glob("*.xml"))


class DeviceTypesTest(unittest.TestCase):
    def test_device_descriptions_parse(self):
        """Every device description is well-formed in the encoding it declares."""
        for path in device_files():
            with self.subTest(file=path.name):
                data = path.read_bytes()
                match = DECLARATION.match(data)
                if match and match.group(1).decode().lower() in ("utf-8", "utf8"):
                    try:
                        data.decode("utf-8")
                    except UnicodeDecodeError as error:
                        self.fail(f"{path.name} declares UTF-8 but is not: {error}")
                try:
                    ET.fromstring(data)
                except ET.ParseError as error:
                    self.fail(f"{path.name} is not well-formed: {error}")

    def test_device_descriptions_indented_with_spaces(self):
        """No device description of the HB devices uses non-breaking spaces."""
        for path in sorted((DEVICETYPES / "rftypes").glob("hb*.xml")):
            with self.subTest(file=path.name):
                self.assertNotIn(b"\xa0", path.read_bytes())

    def test_devdescr_images_exist(self):
        """Every image of a device database entry exists in the WebUI."""
        for path in sorted(DEVDESCR.glob("*.tcl")):
            text = path.read_text(encoding="iso-8859-1")
            for image in IMAGE.findall(text):
                with self.subTest(file=path.name, image=image):
                    self.assertTrue((WWW / image.lstrip("/")).is_file(),
                                    f"{path.name}: {image} does not exist")


if __name__ == "__main__":
    unittest.main()
