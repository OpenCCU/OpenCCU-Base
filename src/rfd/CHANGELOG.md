# rfd ChangeLog

Changes of `rfd`, the BidCos-RF interface process, including the changes of
the libraries it is built with (libXmlRpc, libhsscomm, libelvutils,
libLanDeviceUtils, libUnifiedLanComm, libxmlparser). Newest version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`version.sh` and
`generated-rfd-version.h`).

## 2.6.8 (2026-11-01)

* fix a request to the radio module getting the answer of another one: when
  two system commands or two BidCoS requests were sent to the coprocessor at
  the same time, the second overwrote the first while it waited for its
  answer, so the first caller got the second caller's answer (e.g.
  `getDutyCycle()` succeeding without a value) and the second one timed out.
  Requests of each kind are now sent one after the other, and a second
  request waits for the first instead of failing.
  ([#282](https://github.com/OpenCCU/OpenCCU-Base/pull/282),
  fixes [#179](https://github.com/OpenCCU/OpenCCU-Base/issues/179))
* fix a logic layer (e.g. ReGaHss) that registers after a device replacement
  getting `deleteDevices` for the old device instead of `replaceDevice`, so
  that it lost the names, rooms and programs of that device: the replacement
  check of rfd always answered "not replaced", a replacement was only known
  after a restart, and `replaceDevice` was sent to every registered receiver.
  A replacement is now known at once, also for a device replaced more than
  once, and only the registering receiver gets `replaceDevice(old, new)`.
  ([#283](https://github.com/OpenCCU/OpenCCU-Base/pull/283),
  fixes [#169](https://github.com/OpenCCU/OpenCCU-Base/issues/169))
* handle SIGTERM and SIGINT with a clean shutdown: rfd was killed by the
  default action when the init system or monit stopped it, so a device save
  still pending was lost and the radio module or LAN gateway was not closed.
  A signal now ends the main loop, timers already due (e.g. a device save
  requested by the last RPC call) are run, and the interfaces are closed.
  The receive threads of HM-CFG-USB and HM-CFG-LAN are now stopped before
  their interface is destroyed, which also fixes an abort of libusb and a
  deadlock in `StopInterface()`.
  ([#255](https://github.com/OpenCCU/OpenCCU-Base/pull/255),
  fixes [#164](https://github.com/OpenCCU/OpenCCU-Base/issues/164))
* fix the RPC server no longer answering, or rfd aborting, once one of its
  sockets got a file descriptor of 1024 or more: `select()` writes past its
  descriptor set for such descriptors. rfd (libXmlRpc, the serial port and
  socket wrappers and the HM-CFG-LAN discovery) now waits with `poll()`.
  ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285),
  fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178))
* an interrupted `select()` (e.g. by the stop signal) is no longer logged as
  an error. ([#281](https://github.com/OpenCCU/OpenCCU-Base/pull/281))

## 2.6.7 (2026-10-31)

* fix the RSSI of a device's acknowledgement (`RSSI_PEER`, `rssiInfo`) being
  wrong, e.g. -216 instead of -40 dBm on ARM and +40 on x86_64: the send
  status of the radio module is now read as a signed byte.
  ([#254](https://github.com/OpenCCU/OpenCCU-Base/pull/254),
  fixes [#185](https://github.com/OpenCCU/OpenCCU-Base/issues/185))
* fix rfd using a full CPU core and still reporting the interface as
  connected when the serial device of the radio module hung up (end of file,
  e.g. an unplugged USB serial adapter): end of file is now handled like a
  read error, logged once and reported as not connected until a read
  succeeds again.
  ([#264](https://github.com/OpenCCU/OpenCCU-Base/pull/264),
  fixes [#162](https://github.com/OpenCCU/OpenCCU-Base/issues/162))
* fix an RPC call with fewer arguments than the method expects (e.g.
  `setVolatileMetadata("x")`) reading freed memory and possibly crashing rfd.
  Methods whose arguments are all required (e.g. `getParamset`, `putParamset`,
  `getValue`, `setValue`, `getMetadata`) now answer with the fault -1
  "Insufficient parameters.".
  ([#271](https://github.com/OpenCCU/OpenCCU-Base/pull/271),
  fixes [#167](https://github.com/OpenCCU/OpenCCU-Base/issues/167))
* correct the list of log levels in the sample `rfd.conf` (default 3 =
  NOTICE). ([#277](https://github.com/OpenCCU/OpenCCU-Base/pull/277),
  fixes [#8](https://github.com/OpenCCU/OpenCCU-Base/issues/8))

## 2.6.6 (2026-10-30)

* fix `setBidcosInterface()` with an interface that is not configured (a
  typo, an interface removed from `rfd.conf`, a replaced HM-LGW) storing that
  interface for the device while using the default interface: such a call is
  now answered with the fault -2 "Unknown interface" and changes nothing.
  ([#272](https://github.com/OpenCCU/OpenCCU-Base/pull/272),
  fixes [#174](https://github.com/OpenCCU/OpenCCU-Base/issues/174))
* fix events (key presses, status reports, duty cycle) getting lost under
  load: the unread input of the serial device was discarded before every
  command to the radio module.
  ([#274](https://github.com/OpenCCU/OpenCCU-Base/pull/274),
  fixes [#173](https://github.com/OpenCCU/OpenCCU-Base/issues/173))
* fix the event dispatcher thread of a receiver being joined twice when the
  receiver was removed, and the main loop hanging when a receiver was removed
  right after it was added (libelvutils).
  ([#269](https://github.com/OpenCCU/OpenCCU-Base/pull/269),
  fixes [#161](https://github.com/OpenCCU/OpenCCU-Base/issues/161))

## 2.6.5 (2026-10-30)

* fix a read past the end of the duty cycle threshold map when the radio
  module reported a duty cycle of 100 %. The thresholds and `DUTY_CYCLE`
  events are unchanged.
  ([#266](https://github.com/OpenCCU/OpenCCU-Base/pull/266),
  fixes [#172](https://github.com/OpenCCU/OpenCCU-Base/issues/172))
* fix rfd hanging at start at 100 % CPU when a line of `rfd.conf` or of the
  key and address files had 256 characters or more (libelvutils).
  ([#263](https://github.com/OpenCCU/OpenCCU-Base/pull/263),
  fixes [#177](https://github.com/OpenCCU/OpenCCU-Base/issues/177))
* fix a wrong conversion of long strings to unsigned integers (used by the
  HM-OU-LED16 at channel set-up) in libhsscomm.
  ([#268](https://github.com/OpenCCU/OpenCCU-Base/pull/268),
  fixes [#168](https://github.com/OpenCCU/OpenCCU-Base/issues/168))

## 2.6.4 (2026-10-29)

* fix rfd terminating (SIGABRT) on a JSON value whose last member is empty,
  e.g. `USER_COLOR` or `USER_PROGRAM` of the HM-LC-RGBW-WM set from the
  WebUI's RGBW dialog with its last field left empty. Such a member is now
  taken as an empty value.
  ([#260](https://github.com/OpenCCU/OpenCCU-Base/pull/260),
  fixes [#175](https://github.com/OpenCCU/OpenCCU-Base/issues/175))

## 2.6.3 (2026-10-28)

* remove the RPC method `exit` from normal builds: it stopped rfd without
  any parameter or authentication for every client that could reach its
  port. It is only compiled in with `DEBUG_SHUTDOWN`.
  ([#258](https://github.com/OpenCCU/OpenCCU-Base/pull/258),
  fixes [#163](https://github.com/OpenCCU/OpenCCU-Base/issues/163))
* fix the AES policy of the channels not being restored after a coprocessor
  re-init (a LAN gateway reconnect, a module restart), so that events of a
  remote with AES active were rejected until the next restart.
  ([#211](https://github.com/OpenCCU/OpenCCU-Base/pull/211),
  fixes [#165](https://github.com/OpenCCU/OpenCCU-Base/issues/165))
* fix a logic layer that accepts a connection but never answers freezing rfd
  (init callbacks) or making it grow by a thread and a socket for every
  callback (asynchronous callbacks): these calls now time out after 30 s
  (libhsscomm).
  ([#207](https://github.com/OpenCCU/OpenCCU-Base/pull/207),
  fixes [#170](https://github.com/OpenCCU/OpenCCU-Base/issues/170);
  [#208](https://github.com/OpenCCU/OpenCCU-Base/pull/208),
  fixes [#171](https://github.com/OpenCCU/OpenCCU-Base/issues/171))
* fix crashes (SIGSEGV) of the RPC server on a nested `system.multicall`, on
  invalid length fields of a BIN-RPC request and on deeply nested values,
  each reachable with a single unauthenticated request (libXmlRpc). Nesting
  is now limited to 64 levels.
  ([#138](https://github.com/OpenCCU/OpenCCU-Base/pull/138),
  [#140](https://github.com/OpenCCU/OpenCCU-Base/pull/140),
  [#189](https://github.com/OpenCCU/OpenCCU-Base/pull/189))

## 2.6.2 (2026-10-03)

* report a serial device with a persistent read error (e.g. a hung up
  `/dev/mmd_bidcos` after a restart of multimacd) as not connected in
  `listBidcosInterfaces()`.
  ([#116](https://github.com/OpenCCU/OpenCCU-Base/pull/116))

## 2.6.1 (2026-10-03)

* fix rfd crashing on every restart of multimacd: a failed `read()` of the
  serial device was taken as 4 GiB of data. A read error is now logged once
  and rfd keeps running. ([#115](https://github.com/OpenCCU/OpenCCU-Base/pull/115))
* fix a crash of the RPC client when a reconnect to a logic layer failed
  (libXmlRpc). ([#27](https://github.com/OpenCCU/OpenCCU-Base/pull/27),
  fixes [OpenCCU/OpenCCU#4138](https://github.com/OpenCCU/OpenCCU/issues/4138))
* fix unsafe array handling and missing read checks in libxmlparser.
  ([#29](https://github.com/OpenCCU/OpenCCU-Base/pull/29))

## 2.6.0 (2026-09-06)

* fix AES for BidCos devices.
  ([#4](https://github.com/OpenCCU/OpenCCU-Base/pull/4))
* improve the reconnect of a LAN gateway (HM-LGW): recover from failed
  reconnects, synchronize the lifetime of the gateway controller during a
  reconnect, and match the gateway serial case-insensitively between
  connection and status.
  ([#13](https://github.com/OpenCCU/OpenCCU-Base/pull/13),
  [#16](https://github.com/OpenCCU/OpenCCU-Base/pull/16),
  [#17](https://github.com/OpenCCU/OpenCCU-Base/pull/17))
* wait with `poll()` instead of `select()` in the LAN gateway connection
  handling (libUnifiedLanComm).
  ([#1](https://github.com/OpenCCU/OpenCCU-Base/pull/1))
* update libxmlparser to version 1.13.
  ([#19](https://github.com/OpenCCU/OpenCCU-Base/pull/19))
* build with CMake. ([#3](https://github.com/OpenCCU/OpenCCU-Base/pull/3))

## Older versions

rfd 2.5.1 and earlier are the versions of eQ-3's OCCU. Their changes are
only recorded in the git history of this repository.
