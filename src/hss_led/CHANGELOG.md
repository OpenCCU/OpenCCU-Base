# hss_led ChangeLog

Changes of `hss_led`, the LED controller and status monitor of the CCU
(`hss_ledctl` is its client invocation). Newest version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`version.sh`).

## 2.3.0 (2026-11-02)

* first version kept in `version.sh` (the version was a fixed "2.3" before).
  `--version` / `-V` also prints the versions of the libraries it uses
  (libXmlRpc, libelvutils); hss_led no longer links libxmlparser, which it
  does not use.
  ([#294](https://github.com/OpenCCU/OpenCCU-Base/pull/294), @jens-maus)
* fix the UDP channel no longer working once its socket got a file
  descriptor of 1024 or more: it waits with `poll()` instead of `select()`.
  ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285), fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178),
  @jens-maus)
* report an active `/etc/config/disableLED` lock in the radio LED status
  (`disabled=0|1` in the RPI-RF-MOD status reply) and mention it in the
  `hss_ledctl` help: with the lock, the LED stays off, manual overrides
  included. ([#203](https://github.com/OpenCCU/OpenCCU-Base/pull/203), refs
  [OpenCCU/OpenCCU#4443](https://github.com/OpenCCU/OpenCCU/issues/4443),
  @jens-maus)
* run the LED controller and the status calculation in one process, with
  `hss_ledctl` as its client invocation, explicit LED-class target selection
  and a minimal recovery build of the same controller.
  ([#59](https://github.com/OpenCCU/OpenCCU-Base/pull/59), @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
