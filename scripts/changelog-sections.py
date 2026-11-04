#!/usr/bin/env python3
"""Print the changelog entries of the OpenCCU-Base components that are new
between two git revisions, as Markdown for release notes.

Usage: scripts/changelog-sections.py [--limit N] [--link-base URL] <old> <new>

Every src/<component>/CHANGELOG.md of <new> is compared with the same
file of <old>. An entry (a "* " list item) that the old file does not
contain is printed under its component and the heading of its section
(e.g. "2.6.9 (2026-11-10)" or "Unreleased"), so a renamed section or a version
bump does not repeat entries that were already part of <old> (the
authors at the end of an entry, "@login", are ignored for this). A
changelog that does not exist in <old> contributes the sections dated
on or after the commit date of <old>, and its "Unreleased" section.

The output starts with "## Component changes" and uses "###" for the
components, "####" for their sections and "#####" for subsections. It is
empty if nothing is new.
"""

import argparse
import re
import subprocess

SKIPPED_SECTIONS = ("Older versions",)
DATE_RE = re.compile(r"\((\d{4}-\d{2}-\d{2})\)\s*$")
AUTHORS_RE = re.compile(r"(, | \()@[A-Za-z0-9-]+(?:, @[A-Za-z0-9-]+)*\)$")


def git(*args):
    """Run git with the given arguments and return its standard output."""
    return subprocess.run(["git", *args], check=True, capture_output=True,
                          text=True).stdout


def show(revision, path):
    """Return the content of path at revision, or None if it does not exist."""
    try:
        return git("show", f"{revision}:{path}")
    except subprocess.CalledProcessError:
        return None


def changelogs(revision):
    """Return the paths of the component changelogs at revision, by name."""
    paths = git("ls-tree", "-r", "--name-only", revision, "--", "src").split()
    return sorted((p for p in paths
                   if re.fullmatch(r"src/[^/]+/CHANGELOG\.md", p)),
                  key=lambda p: p.split("/")[1].lower())


def sections(text):
    """Return [(heading, groups), ...] of the "## " sections, where groups is
    [(subheading, paragraphs, entries), ...] for the "### " subsections (the
    first group has the subheading None)."""
    result = []
    groups = None
    block = None
    for line in text.splitlines():
        if line.startswith("## "):
            groups = [[None, [], []]]
            result.append((line[3:].strip(), groups))
            block = None
        elif groups is None:
            continue
        elif line.startswith("### "):
            groups.append([line[4:].strip(), [], []])
            block = None
        elif line.startswith("* "):
            block = [line]
            groups[-1][2].append(block)
        elif not line.strip():
            if block is not None and block[0].startswith("* "):
                continue
            block = None
        elif block is not None and (line.startswith("  ") or not block[0].startswith("* ")):
            block.append(line)
        else:
            block = [line]
            groups[-1][1].append(block)
    return [(h, [(sub, ["\n".join(p) for p in paras], ["\n".join(e) for e in ents])
                 for sub, paras, ents in gs])
            for h, gs in result if h not in SKIPPED_SECTIONS]


def normalize(entry):
    """Return entry with its whitespace collapsed and without its authors at
    the end ("([#1](...), @a, @b)" or "(@a)"), for comparisons, so that adding
    the authors to an entry later does not make it a new one."""
    return AUTHORS_RE.sub(lambda m: ")" if m.group(1) == ", " else "",
                          " ".join(entry.split()))


def new_entries(old_text, new_text, cutoff):
    """Return the sections of new_text with only their new entries."""
    if old_text is None:
        known = set()
        keep = lambda h: (h == "Unreleased" or
                          (DATE_RE.search(h) and DATE_RE.search(h).group(1) >= cutoff))
    else:
        known = {normalize(e) for _, gs in sections(old_text)
                 for _, _, es in gs for e in es}
        keep = lambda h: True
    picked = []
    for heading, groups in sections(new_text):
        if not keep(heading):
            continue
        kept = []
        for sub, paras, entries in groups:
            added = [e for e in entries if normalize(e) not in known]
            if added:
                kept.append((sub, paras, added))
        if kept:
            picked.append((heading, kept))
    return picked


def main():
    """Print the new component changelog entries between two revisions."""
    parser = argparse.ArgumentParser(
        description=__doc__.split("\n\n")[0],
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("old", help="old revision, e.g. the previous release tag")
    parser.add_argument("new", help="new revision, e.g. HEAD")
    parser.add_argument("--limit", type=int, default=None,
                        help="maximum length of the output in characters: "
                             "the largest components are replaced by a link "
                             "to their changelog until it fits")
    parser.add_argument("--link-base", default="",
                        help="URL the changelog paths are appended to for "
                             "these links, e.g. https://github.com/OpenCCU/"
                             "OpenCCU-Base/blob/<tag>")
    args = parser.parse_args()
    cutoff = git("log", "-1", "--format=%cs", args.old).strip()
    blocks = []
    for path in changelogs(args.new):
        picked = new_entries(show(args.old, path), show(args.new, path), cutoff)
        if not picked:
            continue
        component = path.split("/")[1]
        out = [f"### {component}\n"]
        for heading, groups in picked:
            out.append(f"#### {heading}\n")
            for sub, paras, entries in groups:
                if sub:
                    out.append(f"##### {sub}\n")
                for para in paras:
                    out.append(para + "\n")
                out.append("\n".join(entries) + "\n")
        link = f"{args.link_base.rstrip('/')}/{path}" if args.link_base else path
        short = (f"### {component}\n\n"
                 f"See [{path}]({link}) for the changes.\n")
        blocks.append(["\n".join(out), short])
    header = "## Component changes\n\n"
    if args.limit is not None:
        for block in sorted(blocks, key=lambda b: len(b[0]), reverse=True):
            total = len(header) + sum(len(b[0]) + 1 for b in blocks)
            if total <= args.limit:
                break
            block[0] = block[1]
    if blocks:
        print(header + "\n".join(b[0] for b in blocks), end="")


if __name__ == "__main__":
    main()
