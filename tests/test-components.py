#!/usr/bin/env python3
"""Build and install small selections using the real CMake dependency graph."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

SOURCE = Path(__file__).resolve().parents[1]


class ComponentsTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix="base-components-")
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def build(self, *options):
        def run(*args, **kwargs):
            result = subprocess.run(args, text=True, stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT, **kwargs)
            self.assertEqual(result.returncode, 0, result.stdout[-12000:])
        build = self.root / "build"
        run("cmake", "-S", str(SOURCE), "-B", str(build),
            "-DBUILD_DEFAULT_COMPONENTS=OFF", "-DDEPLOY_TO_REPO=OFF", *options)
        run("cmake", "--build", str(build), "--target", "core", "-j4")
        install = self.root / ("install-" + str(len(list(self.root.glob("install-*")))))
        run("cmake", "--install", str(build), "--prefix", "/", "--component", "runtime",
            env=dict(os.environ, DESTDIR=str(install)))
        return {str(p.relative_to(install)) for p in install.rglob("*") if p.is_file()}

    def test_no_components(self):
        self.assertEqual(self.build(), set())

    def test_ssdp_only_has_no_internal_libraries(self):
        self.assertEqual(self.build("-DBUILD_SSDPD=ON"), {"bin/ssdpd"})

    def test_configuration_tool_closure_and_reselection(self):
        self.assertEqual(self.build("-DBUILD_EQ3CONFIGCMD=ON"), {
            "bin/eq3configcmd", "lib/libeq3config.so", "lib/libelvutils.so",
            "lib/libLanDeviceUtils.so", "lib/libUnifiedLanComm.so"})
        # Old staging files must never leak into installation after deselection.
        self.assertEqual(self.build("-DBUILD_EQ3CONFIGCMD=OFF", "-DBUILD_SSDPD=ON"),
                         {"bin/ssdpd"})

    def test_compatibility_libraries_only(self):
        self.assertEqual(self.build("-DBUILD_COMPAT_LIBRARIES=ON"),
                         {"lib/libxmlparser.so", "lib/libXmlRpc.so"})


if __name__ == "__main__":
    unittest.main()
