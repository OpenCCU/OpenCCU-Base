#!/usr/bin/env python3
"""Give every OpenCCU-Base component with unreleased changes its version for
a release.

Usage: scripts/release-bump.py [--date YYYY-MM-DD] [--bump COMPONENT=LEVEL ...]
                               [--summary FILE] [--dry-run] <release>

For every src/<component>/CHANGELOG.md whose "## Unreleased" section has
entries:

* a component with a version of its own gets a new version, which is written
  to the places named in the header of its changelog (version.sh,
  generated-<daemon>-version.h, the version header of a library,
  MODULE_VERSION, the Tcl package version, ...), and "## Unreleased" becomes
  "## <version> (<date>)";
* a component without a version of its own (webui, devicetypes, hmipserver)
  gets the OpenCCU-Base release instead: "## <release> (<date>)".

The new version:

* is the version in the sources if it is already newer than the last
  version in the changelog (bumped by hand in a pull request);
* otherwise raises the last released version by LEVEL, given with --bump
  (major, minor or patch), or else by "minor" if an entry adds something
  (it starts with "add") and by "patch" otherwise. A two-part version
  (e.g. "0.12", "1.6") raises its last part for both minor and patch.

The ReGaHss changelog is maintained by the release workflow of the ReGaHss
repository and is never changed here. Files are edited byte by byte, so
line endings and encodings are kept.

--summary writes a Markdown table of the changes, --dry-run only prints it.
"""

import argparse
import datetime
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "src"

# components without a version of their own: they get the release
RELEASE_VERSIONED = ("webui", "devicetypes", "hmipserver")
# changelogs that are not maintained in this repository
SKIPPED = ("ReGaHss",)

VERSION_RE = r"\d+(?:\.\d+)+"
UNRELEASED_RE = re.compile(rb"^## Unreleased[ \t]*\r?$", re.M)
HEADING_RE = re.compile(rb"^## (.+?)[ \t]*\r?$", re.M)


class Location:
    """A version in a source file: the regular expression has three groups,
    the text before the version, the version and the text after it."""

    def __init__(self, path, pattern, fmt=None):
        self.path = path
        self.pattern = re.compile(pattern.encode(), re.M)
        self.fmt = fmt

    def read(self, component):
        data = (SRC / component / self.path).read_bytes()
        matches = self.pattern.findall(data)
        if len(matches) != 1:
            raise SystemExit(f"{component}: expected one version in {self.path}, "
                             f"found {len(matches)}")
        return matches[0][1].decode()

    def write(self, component, version):
        path = SRC / component / self.path
        data = path.read_bytes()
        data, count = self.pattern.subn(
            lambda m: m.group(1) + version.encode() + m.group(3), data)
        assert count == 1
        path.write_bytes(data)


class VersionSh:
    """MAJOR/MINOR/PATCH in version.sh."""

    path = "version.sh"

    @staticmethod
    def _pattern(name):
        return re.compile(rb'^(' + name.encode() + rb'=")(\d+)(")', re.M)

    def read(self, component):
        data = (SRC / component / self.path).read_bytes()
        parts = []
        for name in ("MAJOR", "MINOR", "PATCH"):
            matches = self._pattern(name).findall(data)
            if len(matches) != 1:
                raise SystemExit(f"{component}: expected one {name} in version.sh")
            parts.append(matches[0][1].decode())
        return ".".join(parts)

    def write(self, component, version):
        path = SRC / component / self.path
        data = path.read_bytes()
        for name, value in zip(("MAJOR", "MINOR", "PATCH"), version.split(".")):
            data, count = self._pattern(name).subn(
                lambda m, v=value: m.group(1) + v.encode() + m.group(3), data)
            assert count == 1
        path.write_bytes(data)


def define(path, macro):
    return Location(path, r'^(#define\s+' + macro + r'\s+")(' + VERSION_RE + r')(")')


def module_version(path):
    return Location(path, r'^(MODULE_VERSION\(\s*")(' + VERSION_RE + r')("\s*\);)')


def xmlparser_doc(path):
    return Location(path, r'^([ \t]*\*[ \t]*@version[ \t]+V)(' + VERSION_RE + r')(\b)')


# where the version of each component is kept; the first location is read,
# all are written
VERSIONS = {
    "rfd": [VersionSh(), define("generated-rfd-version.h", "RFD_VERSION")],
    "hs485d": [VersionSh(), define("generated-hs485d-version.h", "HS485D_VERSION")],
    "libXmlRpc": [define("src/xmlrpc_version.h", "XMLRPC_VERSION_NUMBER")],
    "libxmlparser": [define("xmlparser_version.h", "XMLPARSER_VERSION_NUMBER"),
                     xmlparser_doc("xmlParser.h"), xmlparser_doc("xmlParser.cpp")],
    "tclrega": [define("tclrega.cpp", "TCLREGA_VERSION")],
    "tclrpc": [define("tclrpc.cpp", "TCLRPC_VERSION")],
    "tcl_homematic": [
        Location("src/trailer.tcl", r'^(package provide HomeMatic )(' + VERSION_RE + r')(\b)'),
        Location("pkgIndex.tcl", r'^(package ifneeded HomeMatic )(' + VERSION_RE + r')(\b)'),
    ],
    "eq3_char_loop": [module_version("eq3_char_loop.c")],
    "bcm2835_raw_uart": [module_version("bcm2835_raw_uart.c")],
}


def locations(component):
    if component in VERSIONS:
        return VERSIONS[component]
    if (SRC / component / "version.sh").is_file():
        return [VersionSh()]
    raise SystemExit(f"{component}: unknown version location, add it to "
                     "VERSIONS in scripts/release-bump.py")


def parse(version):
    return tuple(int(p) for p in version.split("."))


def raise_version(version, level):
    parts = list(parse(version))
    if level == "major":
        parts = [parts[0] + 1] + [0] * (len(parts) - 1)
    elif len(parts) == 2 or level == "patch":
        parts[-1] += 1
    else:  # minor of a three-part version
        parts[1] += 1
        parts[2:] = [0] * (len(parts) - 2)
    return ".".join(str(p) for p in parts)


def unreleased(data):
    """Return (start, end) of the "## Unreleased" heading line and the text of
    its section, or None."""
    match = UNRELEASED_RE.search(data)
    if not match:
        return None
    following = HEADING_RE.search(data, match.end())
    section = data[match.end():following.start() if following else len(data)]
    return match, section


def last_release(data, after):
    """Return the version of the first versioned "## " heading after offset."""
    for match in HEADING_RE.finditer(data, after):
        version = re.match(VERSION_RE, match.group(1).decode("latin-1"))
        if version:
            return version.group(0)
    return None


def entries(section):
    return re.findall(rb"^\* (.*)", section, re.M)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("release", help="OpenCCU-Base release, e.g. 3.89.12")
    parser.add_argument("--date", default=datetime.datetime.now(datetime.timezone.utc)
                        .strftime("%Y-%m-%d"), help="release date (default: today, UTC)")
    parser.add_argument("--bump", action="append", default=[], metavar="COMPONENT=LEVEL",
                        help="bump level (major, minor, patch) of a component; "
                        "can be given several times or comma separated")
    parser.add_argument("--summary", type=Path, help="write a Markdown summary to this file")
    parser.add_argument("--dry-run", action="store_true", help="change no files")
    args = parser.parse_args()

    if not re.fullmatch(VERSION_RE, args.release):
        parser.error(f"invalid release: {args.release}")
    if not re.fullmatch(r"\d{4}-\d{2}-\d{2}", args.date):
        parser.error(f"invalid date: {args.date}")

    levels = {}
    for item in ",".join(args.bump).split(","):
        if not item.strip():
            continue
        component, _, level = item.strip().partition("=")
        if level not in ("major", "minor", "patch"):
            parser.error(f"invalid --bump {item!r}: expected COMPONENT=major|minor|patch")
        if not (SRC / component / "CHANGELOG.md").is_file():
            parser.error(f"invalid --bump {item!r}: no changelog src/{component}/CHANGELOG.md")
        if component in SKIPPED or component in RELEASE_VERSIONED:
            parser.error(f"invalid --bump {item!r}: {component} has no version of its own here")
        levels[component] = level

    # plan and check every change first, so that an error changes no file
    rows = []
    writes = []
    for changelog in sorted(SRC.glob("*/CHANGELOG.md"), key=lambda p: p.parent.name.lower()):
        component = changelog.parent.name
        data = changelog.read_bytes()
        found = unreleased(data)
        if component in SKIPPED or not found:
            continue
        heading, section = found
        if not entries(section):
            print(f"{component}: \"Unreleased\" has no entries, left as it is",
                  file=sys.stderr)
            continue

        if component in RELEASE_VERSIONED:
            old, version, reason = "", args.release, "OpenCCU-Base release"
        else:
            where = locations(component)
            current = where[0].read(component)
            for location in where[1:]:
                other = location.read(component)
                if other != current:
                    raise SystemExit(f"{component}: version {other} in {location.path} "
                                     f"differs from {current} in {where[0].path}")
            old = last_release(data, heading.end())
            if old is None or parse(current) > parse(old):
                version, reason = current, "already raised in the sources"
            else:
                if current != old:
                    raise SystemExit(f"{component}: version {current} in the sources is "
                                     f"older than {old} in the changelog")
                level = levels.get(component)
                reason = "--bump"
                if level is None:
                    adds = any(re.match(rb"adds?\b", e, re.I) for e in entries(section))
                    level = "minor" if adds else "patch"
                    reason = "an entry adds something" if adds else "fixes and changes"
                version = raise_version(old, level)
                reason = f"{level} ({reason})"
            if version != current:
                writes += [(location.write, component, version) for location in where]

        new_heading = f"## {version} ({args.date})".encode()
        line_end = b"\r" if heading.group(0).endswith(b"\r") else b""
        writes.append((Path.write_bytes, changelog, data[:heading.start()] + new_heading
                       + line_end + data[heading.end():]))
        rows.append((component, old or "", version, len(entries(section)), reason))

    unknown = sorted(set(levels) - {row[0] for row in rows})
    if unknown:
        raise SystemExit(f"--bump for components without unreleased changes: {', '.join(unknown)}")

    if not args.dry_run:
        for write, *arguments in writes:
            write(*arguments)

    lines = ["| Component | Previous | New | Entries | Reason |",
             "| --- | --- | --- | ---: | --- |"]
    lines += [f"| {c} | {o} | {n} | {e} | {r} |" for c, o, n, e, r in rows]
    text = "\n".join(lines) + "\n" if rows else "No component has unreleased changes.\n"
    print(text, end="")
    if args.summary:
        args.summary.write_text(text)


if __name__ == "__main__":
    main()
