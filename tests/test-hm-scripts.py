#!/usr/bin/env python3
"""bin/ holds copies of the ReGa scripts in src/webui/rega/bin for OpenCCU."""
import os
from pathlib import Path
import unittest

SOURCE = Path(__file__).resolve().parents[1]
SCRIPTS = ("hm_autoconf", "hm_deldev", "hm_startup")


class HmScriptsTest(unittest.TestCase):
    def test_copies_in_bin_equal_sources(self):
        """Every script in bin/ is a byte-exact, executable copy of its source."""
        for name in SCRIPTS:
            with self.subTest(script=name):
                source = SOURCE / "src/webui/rega/bin" / name
                copy = SOURCE / "bin" / name
                self.assertEqual(
                    copy.read_bytes(), source.read_bytes(),
                    f"bin/{name} differs from src/webui/rega/bin/{name}: edit the "
                    f"source and copy it with: cp -p src/webui/rega/bin/{name} bin/{name}")
                for path in (source, copy):
                    self.assertTrue(os.access(path, os.X_OK), f"{path} is not executable")


if __name__ == "__main__":
    unittest.main()
