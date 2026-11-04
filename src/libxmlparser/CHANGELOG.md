# libxmlparser ChangeLog

Changes of `libxmlparser`, the XML parser library of rfd, hs485d, tclrega
and ReGaHss (based on Frank Vanden Berghen's xmlParser). Newest version
first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`XMLPARSER_VERSION_NUMBER` in
`xmlparser_version.h` and `@version` in `xmlParser.h`).

## Unreleased

* add `xmlparser_version()`, which returns the version of the loaded library.
  ([#294](https://github.com/OpenCCU/OpenCCU-Base/pull/294), @jens-maus)

## 1.14 (2026-09-08)

* fix undefined behaviour and a heap overflow when the child arrays of a
  node grow or shrink, and check the results of seek, size, allocation and
  read when a file is parsed.
  ([#29](https://github.com/OpenCCU/OpenCCU-Base/pull/29), @jens-maus)

## 1.13 (2026-09-04)

* update to xmlParser version 1.13.
  ([#19](https://github.com/OpenCCU/OpenCCU-Base/pull/19), @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
