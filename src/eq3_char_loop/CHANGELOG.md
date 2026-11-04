# eq3_char_loop ChangeLog

Changes of `eq3_char_loop`, the kernel module that provides the device nodes
through which multimacd shares the radio module. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`MODULE_VERSION`).

## 1.6 (2026-10-03)

* hang up the connections of rfd and the HmIP server when multimacd (the
  master) exits: they got attached to the next multimacd without it knowing,
  wrote into a buffer nobody read and could block for ever, even against
  SIGKILL. Such a connection now fails with `ENODEV`/`POLLHUP` until it is
  opened again.
  ([#112](https://github.com/OpenCCU/OpenCCU-Base/pull/112), @jens-maus)

## 1.5 (2026-09-05)

* update to version 1.5, including compatibility with Linux 6.
  ([#22](https://github.com/OpenCCU/OpenCCU-Base/pull/22), @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
