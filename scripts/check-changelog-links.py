#!/usr/bin/env python3
"""Check that every new component changelog entry links its change.

Usage: scripts/check-changelog-links.py <old> [<new>]

Every "* " entry of a src/<component>/CHANGELOG.md of <new> (default HEAD)
that the same file of <old> does not contain (a new or reworded entry) must
end with a parenthesis that links at least one pull request, issue or
commit on GitHub, e.g.

  ([#123](https://github.com/OpenCCU/OpenCCU-Base/pull/123), @a)
  (cf. [OpenCCU/OpenCCU#45](https://github.com/OpenCCU/OpenCCU/issues/45))

The release notes of OpenCCU-Base and OpenCCU show every entry as one line
with the links of this parenthesis only, so an entry without one cannot be
traced from the release notes. The authors at the end of the parenthesis are
ignored when entries are compared, like in scripts/changelog-sections.py.

Exits with 1 and lists the entries without a link if there are any.
"""

import importlib.util
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.dont_write_bytecode = True  # no __pycache__ next to the scripts
_spec = importlib.util.spec_from_file_location(
    "changelog_sections", os.path.join(HERE, "changelog-sections.py"))
cs = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(cs)

# a Markdown link to a pull request, an issue or a commit on GitHub
CHANGE_LINK_RE = re.compile(
    r"\[[^\]]+\]\(https://github\.com/[\w.-]+/[\w.-]+/"
    r"(?:pull/\d+|issues/\d+|commit/[0-9a-f]{7,40})[^)\s]*\)")


def has_change_link(entry):
    """Return whether the parenthesis at the end of entry links a change."""
    _, references = cs.split_references(entry)
    return bool(CHANGE_LINK_RE.search(references))


def missing_links(old, new):
    """Return [(path, heading, entry), ...] of the new entries without a link.
    The entries are those that scripts/changelog-sections.py prints for the
    release notes: for a changelog that does not exist in <old>, the entries
    of its "Unreleased" section and of the sections dated on or after the
    commit date of <old>."""
    cutoff = cs.git("log", "-1", "--format=%cs", old).strip()
    result = []
    for path in cs.changelogs(new):
        picked = cs.new_entries(cs.show(old, path), cs.show(new, path), cutoff)
        for heading, groups in picked:
            for _, _, entries in groups:
                result += [(path, heading, e) for e in entries
                           if not has_change_link(e)]
    return result


def main():
    if len(sys.argv) not in (2, 3):
        sys.exit(__doc__.split("\n\n")[1])
    old = sys.argv[1]
    new = sys.argv[2] if len(sys.argv) == 3 else "HEAD"
    missing = missing_links(old, new)
    for path, heading, entry in missing:
        first = " ".join(entry[2:].split())
        if len(first) > 100:
            first = first[:97] + "..."
        print(f"{path} ({heading}): no link to a pull request, issue or "
              f"commit at the end of: {first}")
    if missing:
        print(f"\n{len(missing)} changelog entr{'y' if len(missing) == 1 else 'ies'} "
              "without a link. End every entry with a parenthesis that links "
              "its pull request and the fixed issue, e.g. "
              "\"([#1](https://github.com/OpenCCU/OpenCCU-Base/pull/1), "
              "fixes [#2](https://github.com/OpenCCU/OpenCCU-Base/issues/2), "
              "@author)\".")
        sys.exit(1)
    print("All new changelog entries link their change.")


if __name__ == "__main__":
    main()
