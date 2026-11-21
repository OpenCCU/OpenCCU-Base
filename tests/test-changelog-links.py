#!/usr/bin/env python3
"""scripts/check-changelog-links.py accepts only new changelog entries that
link a pull request, issue or commit at their end."""
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / "scripts/check-changelog-links.py"
PR = "https://github.com/OpenCCU/OpenCCU-Base/pull/1"
ISSUE = "https://github.com/OpenCCU/OpenCCU/issues/2"
COMMIT = "https://github.com/OpenCCU/OpenCCU-Base/commit/0123456789abcdef"

OLD = f"""# rfd ChangeLog

## 2.7.0 (2026-11-01)

* fix an old bug ([#1]({PR}), @a).
* an old entry without a link. (@a)
"""


class ChangelogLinksTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = Path(self.tmp.name)
        env = {"GIT_AUTHOR_NAME": "t", "GIT_AUTHOR_EMAIL": "t@t",
               "GIT_COMMITTER_NAME": "t", "GIT_COMMITTER_EMAIL": "t@t"}
        self.env = {**os.environ, **env}
        self.git("init", "-q")
        self.commit({"src/rfd/CHANGELOG.md": OLD})
        self.base = self.git("rev-parse", "HEAD").strip()

    def tearDown(self):
        self.tmp.cleanup()

    def git(self, *args):
        return subprocess.run(["git", *args], cwd=self.repo, env=self.env,
                              check=True, capture_output=True, text=True).stdout

    def commit(self, files):
        for name, text in files.items():
            path = self.repo / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text)
        self.git("add", "-A")
        self.git("commit", "-q", "-m", "change")

    def check(self):
        return subprocess.run(["python3", str(SCRIPT), self.base], cwd=self.repo,
                              capture_output=True, text=True)

    def add_unreleased(self, entry):
        head, rest = OLD.split("## 2.7.0", 1)
        self.commit({"src/rfd/CHANGELOG.md":
                     f"{head}## Unreleased\n\n{entry}\n\n## 2.7.0{rest}"})

    def test_entries_with_a_link_pass(self):
        for refs in (f"([#3]({PR}), fixes [#2]({ISSUE}), @a)",
                     f"(cf. [OpenCCU/OpenCCU#2]({ISSUE}))",
                     f"([0123456]({COMMIT}), @a)"):
            with self.subTest(refs=refs):
                self.git("reset", "-q", "--hard", self.base)
                self.add_unreleased(f"* fix a new bug.\n  {refs}")
                result = self.check()
                self.assertEqual(result.returncode, 0, result.stdout)

    def test_entries_without_a_link_fail(self):
        for refs in ("(@a)", "", f"(e.g. see [this]({PR}/files)) and more"):
            with self.subTest(refs=refs):
                self.git("reset", "-q", "--hard", self.base)
                self.add_unreleased(f"* fix a new bug. {refs}".rstrip())
                result = self.check()
                self.assertEqual(result.returncode, 1, result.stdout)
                self.assertIn("src/rfd/CHANGELOG.md (Unreleased)", result.stdout)
                self.assertIn("fix a new bug", result.stdout)

    def test_old_entries_and_new_authors_are_not_checked(self):
        # adding an author or renaming the section keeps an entry known
        self.commit({"src/rfd/CHANGELOG.md":
                     OLD.replace("@a).", "@a, @b).")
                        .replace("2.7.0 (2026-11-01)", "2.7.0 (2026-11-02)")})
        result = self.check()
        self.assertEqual(result.returncode, 0, result.stdout)

    def test_new_changelog_checks_only_recent_sections(self):
        self.commit({"src/hs485d/CHANGELOG.md":
                     "# hs485d ChangeLog\n\n## Unreleased\n\n* new entry. (@a)\n\n"
                     "## 1.0.0 (2001-01-01)\n\n* ancient entry. (@a)\n"})
        result = self.check()
        self.assertEqual(result.returncode, 1, result.stdout)
        self.assertIn("new entry", result.stdout)
        self.assertNotIn("ancient entry", result.stdout)


if __name__ == "__main__":
    unittest.main()
