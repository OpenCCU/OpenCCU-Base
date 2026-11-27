# devicetypes ChangeLog

Changes of `devicetypes`, the device descriptions of the BidCos-RF and
BidCos-Wired devices. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
OpenCCU-Base release they are part of when it is published.

## Unreleased

* add the device descriptions of the 86 homebrew device types of the
  [JP-HB-Devices addon](https://github.com/jp112sdl/JP-HB-Devices-addon)
  (`hb-*.xml`, `hb_switch_tp.xml`). A doubled quote in
  `hb-uni-sen-pc-wm.xml`, `hb-uni-sen-press.xml` and
  `hb-uni-sen-press-sc.xml` and the UTF-8 declaration of the ISO-8859-1
  file `hb-lc-dim5pwm-cv.xml` are fixed, and `hb-rc-4-dis-th.xml` and
  `hb-rc-4-dis-th-crg.xml` are indented with spaces instead of
  non-breaking spaces. Device descriptions customized in the
  `customized_firmware` folder of the addon are not taken over.
  ([#414](https://github.com/OpenCCU/OpenCCU-Base/pull/414), @jp112sdl,
  @jens-maus)

## 3.89.12 (2026-11-16)

* fix the error messages `Unknown paramset` / `GetAESMasterOperation()` of
  rfd and ReGaHss when opening the device list or channel settings of an
  HM-Sen-RD-O: its heating channel 2 had no `MASTER` paramset. It now has
  an empty one; AES for this channel stays unavailable as before.
  ([#335](https://github.com/OpenCCU/OpenCCU-Base/pull/335),
  [discussion #4680](https://github.com/orgs/OpenCCU/discussions/4680),
  @jens-maus)
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
