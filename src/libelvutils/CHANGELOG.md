# libelvutils ChangeLog

Changes of `libelvutils`, the utility library (logging, configuration files,
threads, hex files) of the OpenCCU-Base programs. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`version.sh`).

## 1.0.0 (2026-11-02)

* first version with a version string (`version.sh`), returned by
  `elvutils_version()`.
  ([#294](https://github.com/OpenCCU/OpenCCU-Base/pull/294), @jens-maus)
* fix the multicast sender failing once a descriptor was 1024 or more: it
  waits with `poll()` instead of `select()`.
  ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285), fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178),
  @jens-maus)
* fix the thread of an actor (the event dispatcher of each receiver in rfd
  and hs485d) being joined twice, and a hang when an actor was stopped right
  after it was started. ([#269](https://github.com/OpenCCU/OpenCCU-Base/pull/269), fixes [#161](https://github.com/OpenCCU/OpenCCU-Base/issues/161),
  @jens-maus)
* refuse a hex file without data (e.g. an empty or truncated firmware file)
  instead of crashing the reader. ([#267](https://github.com/OpenCCU/OpenCCU-Base/pull/267), fixes [#176](https://github.com/OpenCCU/OpenCCU-Base/issues/176),
  @jens-maus)
* read configuration lines of any length: a line of 256 characters or more
  made the programs hang at start at 100 % CPU, and a read error is reported.
  ([#263](https://github.com/OpenCCU/OpenCCU-Base/pull/263), fixes [#177](https://github.com/OpenCCU/OpenCCU-Base/issues/177),
  @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
