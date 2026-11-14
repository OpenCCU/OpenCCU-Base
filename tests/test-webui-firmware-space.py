#!/usr/bin/env python3
"""Check the free space calculation of the WebUI firmware update.

cp_maintenance.cgi blocks the firmware upload if the userfs has less free
space than an update needs, and checks after the upload whether the recovery
system will have enough space to prepare the uploaded file. The procedures
computing this are run with tclsh, with unzip and dd replaced by mocks.
"""
import os
from pathlib import Path
import re
import shutil
import subprocess
import tarfile
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
CGI = ROOT / "src/webui/www/config/cp_maintenance.cgi"
PROCS = ("firmware_sparse_supported", "get_userfs_free_bytes",
         "get_firmware_required_bytes")
MIB = 1024 * 1024

# "unzip -l" listing of a release zip as printed by BusyBox and by Info-ZIP
BUSYBOX_LISTING = """Archive:  {zip}
  Length      Date    Time    Name
---------  ---------- -----   ----
2419044352  09-19-2026 11:01   OpenCCU-3.89.11-rpi4.img
      111  09-19-2026 11:01   OpenCCU-3.89.11-rpi4.img.sha256
     5596  09-19-2026 08:23   EULA.de
 --------                     -------
2419050059                     3 files
"""
INFOZIP_LISTING = """Archive:  {zip}
  Length      Date    Time    Name
---------  ---------- -----   ----
2419044352  2026-09-19 11:01   OpenCCU 3.89.11 rpi4.img
      111  2026-09-19 11:01   OpenCCU 3.89.11 rpi4.img.sha256
     5596  2026-09-19 08:23   EULA.de
---------                     -------
2419050059                     3 files
"""


def extract_procs(source):
    """The Tcl source of the procedures in PROCS."""
    procs = []
    for name in PROCS:
        match = re.search(r"^proc %s \{.*?^\}$" % name, source,
                          re.MULTILINE | re.DOTALL)
        if not match:
            raise AssertionError(f"proc {name} not found in {CGI}")
        procs.append(match.group(0))
    return "\n\n".join(procs)


@unittest.skipUnless(shutil.which("tclsh") and shutil.which("file"),
                     "tclsh and file are required")
class FirmwareSpaceTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="fw-space-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.bin = self.root / "bin"
        self.bin.mkdir()
        procs = extract_procs(CGI.read_text(encoding="utf-8"))
        procs = procs.replace("/usr/bin/unzip ", f"{self.bin}/unzip ")
        procs = procs.replace("/bin/tar ", "tar ")
        self.procs = self.root / "procs.tcl"
        self.procs.write_text(procs)
        self.env = dict(os.environ,
                        PATH=str(self.bin) + ":" + os.environ["PATH"])
        self.set_sparse(True)

    def set_sparse(self, supported):
        dd = self.bin / "dd"
        rc = 0 if supported else 1
        dd.write_text(f"#!/bin/sh\ncase \"$*\" in *conv=sparse*) exit {rc};;"
                      f" esac\nexec {shutil.which('dd')} \"$@\"\n")
        dd.chmod(0o755)

    def make_zip(self, listing, size):
        """A file detected as zip archive of the given size, whose listing
        the unzip mock prints."""
        path = self.root / "upload.zip"
        with open(path, "wb") as f:
            f.write(b"PK\x03\x04" + b"\0" * 26)
            f.truncate(size)
        unzip = self.bin / "unzip"
        (self.root / "listing").write_text(listing.format(zip=path))
        unzip.write_text(f"#!/bin/sh\ncat {self.root / 'listing'}\n")
        unzip.chmod(0o755)
        return path

    def tcl(self, script):
        result = subprocess.run(
            ["tclsh"], input=f"source {self.procs}\n{script}\n",
            env=self.env, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        return result.stdout.strip()

    def required(self, path):
        return int(self.tcl(f"puts [get_firmware_required_bytes {path}]"))

    def test_zip_image_is_estimated_sparse(self):
        for listing in (BUSYBOX_LISTING, INFOZIP_LISTING):
            path = self.make_zip(listing, 400 * MIB)
            # image estimated with three times the zip size plus the other
            # files with their full size
            self.assertEqual(self.required(path),
                             3 * 400 * MIB + 111 + 5596)

    def test_zip_sparse_estimate_is_capped_at_image_size(self):
        path = self.make_zip(BUSYBOX_LISTING, 900 * MIB)
        self.assertEqual(self.required(path), 2419050059)

    def test_zip_without_sparse_support_needs_full_size(self):
        self.set_sparse(False)
        path = self.make_zip(INFOZIP_LISTING, 400 * MIB)
        self.assertEqual(self.required(path), 2419050059)
        self.assertEqual(self.tcl("puts [firmware_sparse_supported]"), "0")

    def test_zip_without_image_needs_full_size(self):
        listing = BUSYBOX_LISTING.replace("rpi4.img", "rpi4.bin")
        path = self.make_zip(listing, 400 * MIB)
        self.assertEqual(self.required(path), 2419050059)

    def test_unreadable_zip_listing_is_unknown(self):
        path = self.make_zip("garbage\n", MIB)
        self.assertEqual(self.required(path), -1)

    def test_tar_needs_its_content_size(self):
        content = self.root / "update_script"
        content.write_bytes(b"x" * 12345)
        path = self.root / "update.tgz"
        with tarfile.open(path, "w:gz") as tar:
            tar.add(content, "update_script")
            tar.add(content, "rootfs.ext4.gz")
        self.assertEqual(self.required(path), 2 * 12345)

    def test_other_files_need_no_extra_space(self):
        path = self.root / "rootfs.ext4"
        path.write_bytes(b"\0" * 4096)
        self.assertEqual(self.required(path), 0)

    def test_userfs_free_bytes(self):
        free = int(self.tcl("puts [get_userfs_free_bytes]"))
        if Path("/usr/local").exists():
            self.assertGreaterEqual(free, 0)
            self.assertEqual(free % 1024, 0)
        else:
            self.assertEqual(free, -1)


if __name__ == "__main__":
    unittest.main()
