# libeq3config ChangeLog

Changes of `libeq3config`, the library behind eq3configcmd. Newest version
first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`version.sh`).

## 1.0.0 (2026-11-02)

* first version with a version string (`version.sh`), returned by
  `eq3config_version()`.
  ([#294](https://github.com/OpenCCU/OpenCCU-Base/pull/294), @jens-maus)
* fix the serial port wrapper failing once a descriptor was 1024 or more: it
  waits with `poll()` instead of `select()`.
  ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285), fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178),
  @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
