#!/usr/bin/env python3
"""Build and install selections using the real CMake dependency graph."""
import os
from pathlib import Path
import shutil
import signal
import subprocess
import sys
import tempfile
import unittest

SOURCE = Path(__file__).resolve().parents[1]
TIMEOUT = float(os.environ.get("OPENCCU_TEST_TIMEOUT", "600"))


def run(*args, timeout=TIMEOUT, **kwargs):
    """Run a build step, killing its entire process group on timeout."""
    with subprocess.Popen(args, text=True, stdout=subprocess.PIPE,
                          stderr=subprocess.STDOUT, start_new_session=True,
                          **kwargs) as process:
        try:
            output, _ = process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            output, _ = process.communicate()
            raise AssertionError(f"Timed out after {timeout}s: {args}\n{output[-12000:]}") from None
        if process.returncode:
            raise AssertionError(f"Command failed ({process.returncode}): {args}\n{output[-12000:]}")


class ComponentsTest(unittest.TestCase):
    def setUp(self):
        """Keep builds and installation destinations separate from the checkout."""
        self.tmp = tempfile.TemporaryDirectory(prefix="base-components-")
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def build(self, *options, component="runtime", source=SOURCE):
        """Exercise the default build and install into a new DESTDIR."""
        build = self.root / "build"
        run("cmake", "-S", str(source), "-B", str(build),
            "-DBUILD_DEFAULT_COMPONENTS=OFF", "-DDEPLOY_TO_REPO=OFF", *options)
        run("cmake", "--build", str(build), "-j4")
        self.install = self.root / ("install-" + str(len(list(self.root.glob("install-*")))))
        run("cmake", "--install", str(build), "--prefix", "/", "--component", component,
            env=dict(os.environ, DESTDIR=str(self.install)))
        return {str(p.relative_to(self.install)) for p in self.install.rglob("*")
                if p.is_file() or p.is_symlink()}

    def fixture(self):
        """Create a small source wrapper without modifying repository inputs."""
        source = self.root / "source"
        source.mkdir()
        for name in ("CMakeLists.txt", "CMakePresets.json", "Makefile"):
            shutil.copyfile(SOURCE / name, source / name)
        (source / "cmake").symlink_to(SOURCE / "cmake", target_is_directory=True)
        (source / "src").mkdir()
        for path in (SOURCE / "src").iterdir():
            if path.name == "ssdpd":
                shutil.copytree(path, source / "src/ssdpd")
            else:
                (source / "src" / path.name).symlink_to(path, target_is_directory=path.is_dir())
        return source

    def test_no_components(self):
        """An empty selection installs nothing."""
        self.assertEqual(self.build(), set())

    def test_ssdp_only_has_no_internal_libraries(self):
        """Standalone SSDP does not pull in CCU shared libraries."""
        self.assertEqual(self.build("-DBUILD_SSDPD=ON"), {"bin/ssdpd"})

    def test_configuration_tool_closure_and_reselection(self):
        """Deselected native outputs cannot leak from a reused staging tree."""
        self.assertEqual(self.build("-DBUILD_EQ3CONFIGCMD=ON"), {
            "bin/eq3configcmd", "lib/libeq3config.so", "lib/libelvutils.so",
            "lib/libLanDeviceUtils.so", "lib/libUnifiedLanComm.so"})
        self.assertEqual(self.build("-DBUILD_EQ3CONFIGCMD=OFF", "-DBUILD_SSDPD=ON"),
                         {"bin/ssdpd"})

    def test_compatibility_libraries_only(self):
        """The default build includes explicitly selected compatibility libraries."""
        self.assertEqual(self.build("-DBUILD_COMPAT_LIBRARIES=ON"),
                         {"lib/libxmlparser.so", "lib/libXmlRpc.so"})

    def test_asset_only_default_build(self):
        """A default build prepares asset inputs needed by installation."""
        files = self.build("-DBUILD_HMSERVER=ON", component="assets")
        self.assertIn("opt/HMServer/HMServer.jar", files)

    def test_runtime_alias(self):
        """Aliases are relative, relocatable and omitted when deselected."""
        source = self.fixture()
        cmake = source / "src/ssdpd/CMakeLists.txt"
        cmake.write_text(cmake.read_text().replace("openccu_stage_target(ssdpd bin)",
                         "set_property(TARGET ssdpd PROPERTY OPENCCU_RUNTIME_ALIASES ssdp-alias)\n"
                         "openccu_stage_target(ssdpd bin)"))
        self.assertEqual(self.build("-DBUILD_SSDPD=ON", source=source),
                         {"bin/ssdpd", "bin/ssdp-alias"})
        for root in (self.install, self.root / "build/rootfs"):
            self.assertEqual(os.readlink(root / "bin/ssdp-alias"), "ssdpd")
        self.assertIn("bin/ssdp-alias\n", (self.root / "build/runtime-files.txt").read_text())
        # Both reinstallation and re-staging must recreate relative symlinks.
        (self.root / "build/rootfs/bin/ssdp-alias").unlink()
        run("cmake", "--build", str(self.root / "build"))
        self.assertTrue((self.root / "build/rootfs/bin/ssdp-alias").is_symlink())
        self.assertEqual(self.build("-DBUILD_SSDPD=OFF", source=source), set())

    def test_make_install_reselection_preserves_sources(self):
        """Make installs a fresh image; only explicit deployment touches sources."""
        source = self.fixture()
        cmake = source / "src/ssdpd/CMakeLists.txt"
        cmake.write_text(cmake.read_text().replace("openccu_stage_target(ssdpd bin)",
                         "set_property(TARGET ssdpd PROPERTY OPENCCU_RUNTIME_ALIASES ssdp-alias)\n"
                         "openccu_stage_target(ssdpd bin)"))
        protected = [source / p for p in ("bin/x86_64-linux-gnu/ReGaHss", "lib/keep",
                     "etc/keep", "firmware/keep", "www/keep", "opt/keep", "usr/keep")]
        for path in protected:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("repository input")
        def make(target, options):
            run("make", target, "PLATFORMS=x86_64-linux-gnu",
                "CMAKE_OPTIONS=-DBUILD_DEFAULT_COMPONENTS=OFF " + options, cwd=source)
        make("install", "-DBUILD_SSDPD=ON")
        image = source / "build/x86_64-linux-gnu/selected-install"
        self.assertTrue((image / "bin/ssdpd").is_file())
        self.assertFalse((source / "bin/x86_64-linux-gnu/ssdpd").exists())
        make("install", "-DBUILD_SSDPD=OFF")
        self.assertFalse((image / "bin/ssdpd").exists())
        for path in protected:
            self.assertEqual(path.read_text(), "repository input")
        make("deploy", "-DBUILD_SSDPD=ON")
        self.assertTrue((source / "bin/x86_64-linux-gnu/ssdpd").is_file())
        self.assertEqual(os.readlink(source / "bin/x86_64-linux-gnu/ssdp-alias"), "ssdpd")
        for path in protected:
            self.assertEqual(path.read_text(), "repository input")

    def test_timeout_reports_output(self):
        """A stalled child becomes a readable failure instead of a hung test."""
        with self.assertRaisesRegex(AssertionError, "(?s)Timed out.*still running"):
            run(sys.executable, "-c", "import time; print('still running', flush=True); time.sleep(60)",
                timeout=0.5)


if __name__ == "__main__":
    unittest.main()
