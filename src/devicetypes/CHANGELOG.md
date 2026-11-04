# devicetypes ChangeLog

Changes of `devicetypes`, the device descriptions of the BidCos-RF and
BidCos-Wired devices. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
OpenCCU-Base release they are part of when it is published.

## Unreleased

* fix the limits of `SET_TEMPERATURE` (4.5 to 30.5 °C instead of 5.0 to
  30.0 °C) of the HM-CC-RT-DN-BoM, as in the HM-CC-RT-DN, and of the virtual
  heating group HM-CC-VG-1: setting 30.5 °C (on) or 4.5 °C (off) caused
  warnings in integrations such as ioBroker or Home Assistant.
  ([#221](https://github.com/OpenCCU/OpenCCU-Base/pull/221), @jhoos70)
* fix the long-press action of the CFM-TW LED profile writing the
  short-press register (`LONG_ACT_TYPE` at 0xA4 instead of 0x24).
  ([#63](https://github.com/OpenCCU/OpenCCU-Base/pull/63), @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
