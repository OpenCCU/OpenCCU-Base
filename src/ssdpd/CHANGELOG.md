# ssdpd ChangeLog

Changes of `ssdpd`, the SSDP responder that makes the CCU discoverable in
the network. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`version.sh`).

## 1.0.0 (2026-11-02)

* first version with a version string (`version.sh`): `-v` / `--version`
  prints it.
  ([#294](https://github.com/OpenCCU/OpenCCU-Base/pull/294), @jens-maus)
* fix ssdpd failing once its socket got a file descriptor of 1024 or more: it
  waits with `poll()` instead of `select()`.
  ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285), fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178),
  @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
