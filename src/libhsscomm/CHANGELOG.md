# libhsscomm ChangeLog

Changes of `libhsscomm`, the library with the common device and RPC logic of
rfd and hs485d. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`version.sh`).

## 1.0.0 (2026-11-02)

* first version with a version string (`version.sh`), returned by
  `hsscomm_version()`.
  ([#294](https://github.com/OpenCCU/OpenCCU-Base/pull/294), @jens-maus)
* fix the TCP and serial port wrappers failing once a descriptor was 1024 or
  more: they wait with `poll()` instead of `select()`.
  ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285), fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178),
  @jens-maus)
* send `replaceDevice` at the init of a receiver only to that receiver, and do
  not report the channels of a replaced device separately.
  ([#283](https://github.com/OpenCCU/OpenCCU-Base/pull/283), fixes [#169](https://github.com/OpenCCU/OpenCCU-Base/issues/169),
  @jens-maus)
* fix a wrong conversion of long strings to unsigned integers (used by the
  HM-OU-LED16 at channel set-up). ([#268](https://github.com/OpenCCU/OpenCCU-Base/pull/268), fixes [#168](https://github.com/OpenCCU/OpenCCU-Base/issues/168),
  @jens-maus)
* add `UnregisterInternalValueEvents()`, so a channel can remove its event
  registration before it is deleted.
  ([#257](https://github.com/OpenCCU/OpenCCU-Base/pull/257), @jens-maus)
* bound the init callbacks and the asynchronous callbacks to a receiver with
  a 30 s timeout: a receiver that never answers no longer freezes the daemon
  or makes it grow by a thread and a socket per callback.
  ([#207](https://github.com/OpenCCU/OpenCCU-Base/pull/207), fixes [#170](https://github.com/OpenCCU/OpenCCU-Base/issues/170); [#208](https://github.com/OpenCCU/OpenCCU-Base/pull/208), fixes [#171](https://github.com/OpenCCU/OpenCCU-Base/issues/171),
  @hobbyquaker)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
