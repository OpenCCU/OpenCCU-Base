# hs485d ChangeLog

Changes of `hs485d`, the BidCos-Wired interface process, including the
changes of the libraries it is built with (libXmlRpc, libhsscomm,
libelvutils, libLanDeviceUtils, libUnifiedLanComm, libxmlparser). Newest
version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`version.sh` and
`generated-hs485d-version.h`).

## 1.4.0 (2026-11-01)

* add the RPC method `restoreConfigToDevice`, which writes the configuration
  stored on the CCU (paramsets and links) to a device again, e.g. after it was
  reset to factory settings. The "Restore Config" button of the device
  parameters page of the WebUI is now shown for BidCos-Wired devices as well.
  Not yet tested on Wired hardware.
  ([#217](https://github.com/OpenCCU/OpenCCU-Base/pull/217))
* fix a logic layer (e.g. ReGaHss) that registers after a device replacement
  getting `deleteDevices` for the old device instead of `replaceDevice`, so
  that it lost the names, rooms and programs of that device: hs485d stored the
  serial of the new device instead of the old one in the replacement history,
  a replacement was only known after a restart, and `replaceDevice` was sent
  to every registered receiver. A replacement is now known at once, also for a
  device replaced more than once, and only the registering receiver gets
  `replaceDevice(old, new)`. Replacements done with an earlier version are not
  recovered.
  ([#283](https://github.com/OpenCCU/OpenCCU-Base/pull/283),
  fixes [#169](https://github.com/OpenCCU/OpenCCU-Base/issues/169))
* fix a replaced device that is added (paired) again still being reported as
  replaced, so that a logic layer could get `replaceDevice` for it after it
  was deleted later.
  ([#287](https://github.com/OpenCCU/OpenCCU-Base/pull/287))
* handle SIGTERM and SIGINT with a clean shutdown: hs485d was killed by the
  default action when the init system or monit stopped it, so a device save
  still pending was lost. A signal now ends the main loop and the controller
  is stopped and deleted before the logger. This also fixes the crash of the
  shutdown path, which freed the port wrapper twice and the logger while the
  controller's threads were still running.
  ([#255](https://github.com/OpenCCU/OpenCCU-Base/pull/255),
  fixes [#164](https://github.com/OpenCCU/OpenCCU-Base/issues/164))
* fix the RPC server no longer answering, or hs485d aborting, once one of its
  sockets got a file descriptor of 1024 or more: `select()` writes past its
  descriptor set for such descriptors. hs485d (libXmlRpc and the serial port
  and socket wrappers of libhsscomm) now waits with `poll()`.
  ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285),
  fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178))
* an interrupted `select()` (e.g. by the stop signal) is no longer logged as
  an error. ([#281](https://github.com/OpenCCU/OpenCCU-Base/pull/281))

## 1.3.0 (2026-10-31)

* fix a possible use of freed memory when hs485d reconnected to a LAN gateway
  (HMW-LGW) while a value was sent or received: the reconnect now waits for
  running operations before it replaces the gateway controller.
  ([#265](https://github.com/OpenCCU/OpenCCU-Base/pull/265),
  fixes [#195](https://github.com/OpenCCU/OpenCCU-Base/issues/195))
* fix an RPC call with fewer arguments than the method expects reading freed
  memory and possibly crashing hs485d. Methods whose arguments are all
  required (e.g. `getParamset`, `putParamset`, `getValue`, `setValue`) now
  answer with the fault -1 "Insufficient parameters.".
  ([#271](https://github.com/OpenCCU/OpenCCU-Base/pull/271),
  fixes [#167](https://github.com/OpenCCU/OpenCCU-Base/issues/167))
* fix hs485d crashing after every restart when a firmware file of a device
  type had no data (empty or truncated hex file): such a file is now refused
  and no firmware is reported as available.
  ([#267](https://github.com/OpenCCU/OpenCCU-Base/pull/267),
  fixes [#176](https://github.com/OpenCCU/OpenCCU-Base/issues/176))
* fix a use of freed memory after the channels of a device were rebuilt: the
  maintenance channel stayed registered for the events of its device after it
  was deleted.
  ([#257](https://github.com/OpenCCU/OpenCCU-Base/pull/257))
* fix the channels of a newly learned device being created without their
  channel class until hs485d was restarted, so that e.g. `UNREACH`,
  `STICKY_UNREACH` and `CONFIG_PENDING` of channel 0 did not work and a
  failed send left no service message.
  ([#209](https://github.com/OpenCCU/OpenCCU-Base/pull/209),
  fixes [#166](https://github.com/OpenCCU/OpenCCU-Base/issues/166))
* fix a logic layer that accepts a connection but never answers freezing
  hs485d (init callbacks) or making it grow by a thread and a socket for every
  callback (asynchronous callbacks): these calls now time out after 30 s
  (libhsscomm).
  ([#207](https://github.com/OpenCCU/OpenCCU-Base/pull/207),
  fixes [#170](https://github.com/OpenCCU/OpenCCU-Base/issues/170);
  [#208](https://github.com/OpenCCU/OpenCCU-Base/pull/208),
  fixes [#171](https://github.com/OpenCCU/OpenCCU-Base/issues/171))
* fix a wrong conversion of long strings to unsigned integers (used by the
  HM-OU-LED16 at channel set-up) in libhsscomm.
  ([#268](https://github.com/OpenCCU/OpenCCU-Base/pull/268),
  fixes [#168](https://github.com/OpenCCU/OpenCCU-Base/issues/168))
* fix hs485d hanging at start at 100 % CPU when a line of `hs485d.conf` had
  256 characters or more (libelvutils).
  ([#263](https://github.com/OpenCCU/OpenCCU-Base/pull/263),
  fixes [#177](https://github.com/OpenCCU/OpenCCU-Base/issues/177))
* fix the event dispatcher thread of a receiver being joined twice when the
  receiver was removed, and the main loop hanging when a receiver was removed
  right after it was added (libelvutils).
  ([#269](https://github.com/OpenCCU/OpenCCU-Base/pull/269),
  fixes [#161](https://github.com/OpenCCU/OpenCCU-Base/issues/161))
* fix crashes (SIGSEGV) of the RPC server on a nested `system.multicall`, on
  invalid length fields of a BIN-RPC request and on deeply nested values,
  each reachable with a single unauthenticated request (libXmlRpc). Nesting
  is now limited to 64 levels.
  ([#138](https://github.com/OpenCCU/OpenCCU-Base/pull/138),
  [#140](https://github.com/OpenCCU/OpenCCU-Base/pull/140),
  [#189](https://github.com/OpenCCU/OpenCCU-Base/pull/189))
* fix a crash of the RPC client when a reconnect to a logic layer failed
  (libXmlRpc). ([#27](https://github.com/OpenCCU/OpenCCU-Base/pull/27),
  fixes [OpenCCU/OpenCCU#4138](https://github.com/OpenCCU/OpenCCU/issues/4138))
* fix unsafe array handling and missing read checks in libxmlparser.
  ([#29](https://github.com/OpenCCU/OpenCCU-Base/pull/29))

## 1.2.0 (2026-09-06)

* fix the system information of a device in its `.dev` file being corrupted
  on x86_64 for a type byte of 0x80 or more (signed char).
  ([#9](https://github.com/OpenCCU/OpenCCU-Base/pull/9))
* fix the decimal device address in the XML output.
* wait with `poll()` instead of `select()` in the LAN gateway connection
  handling (libUnifiedLanComm).
  ([#1](https://github.com/OpenCCU/OpenCCU-Base/pull/1))
* update libxmlparser to version 1.13.
  ([#19](https://github.com/OpenCCU/OpenCCU-Base/pull/19))
* improve the logging.
* build with CMake. ([#3](https://github.com/OpenCCU/OpenCCU-Base/pull/3))

## 1.1.0 (2025-06-24)

* write all output through the logger instead of `printf()`, and keep a
  version string so that the running hs485d version can be identified.

## Older versions

hs485d versions before 1.1.0 are the versions of eQ-3's OCCU, which had no
version string. Their changes are only recorded in the git history of this
repository.
