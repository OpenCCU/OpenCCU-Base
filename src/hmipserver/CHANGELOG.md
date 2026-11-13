# hmipserver ChangeLog

Changes of `hmipserver`, the HmIP server (`HMIPServer.jar`) of eQ-3, which
is only provided as a binary. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
OpenCCU-Base release they are part of when it is published.

## Unreleased

* update `HMIPServer.jar` to version 1.5.2-SNAPSHOT (build 6140 of
  2026-11-13) from eQ-3. The server again finishes its start while rfd does
  not answer (e.g. a BidCos LAN gateway that is off at boot) and picks rfd
  up later; with 1.5.1 the start stopped, so `HmIP-RF`, `VirtualDevices`
  and the heating groups stayed down until the server was restarted
  ([#360](https://github.com/OpenCCU/OpenCCU-Base/pull/360),
  [homematicip#17](https://github.com/homematicip/OpenCCU-Base/pull/17),
  [#358](https://github.com/OpenCCU/OpenCCU-Base/issues/358), @cn-eq3).
* `HmIP-RF` now removes a registered XML-RPC client after 3 failed calls in
  a row (can be switched off with `Legacy.RemoveUnreachableClients=false`),
  so a client that hangs no longer gets every further event for the full
  `Legacy.Client.Socket.Timeout`, and the `listDevices` calls to newly
  registered clients run in parallel, so a hanging client no longer holds
  up the registration of others
  ([#360](https://github.com/OpenCCU/OpenCCU-Base/pull/360),
  [homematicip#17](https://github.com/homematicip/OpenCCU-Base/pull/17),
  [#192](https://github.com/OpenCCU/OpenCCU-Base/issues/192), @cn-eq3).
* a registration on `VirtualDevices` is no longer kept for ever after it
  was replaced, so its HTTP client can be freed again; it is not closed
  explicitly yet, so its thread and file descriptors stay until the Java
  garbage collector runs
  ([#360](https://github.com/OpenCCU/OpenCCU-Base/pull/360),
  [homematicip#17](https://github.com/homematicip/OpenCCU-Base/pull/17),
  [#36](https://github.com/OpenCCU/OpenCCU-Base/issues/36), @cn-eq3).
* lower `Legacy.Client.Socket.Timeout` in the `crRFD.conf` template from
  600 s to 30 s. Since `HMIPServer.jar` 1.5.1 this is the answer timeout
  of every call to a registered XML-RPC client: a receiver that accepts
  the connection but does not answer held up the registration of other
  clients for 10 minutes and logged a blocked-thread warning every second
  meanwhile; now it gives up after 30 s
  ([#359](https://github.com/OpenCCU/OpenCCU-Base/pull/359),
  [#192](https://github.com/OpenCCU/OpenCCU-Base/issues/192), @jens-maus).
* update `HMIPServer.jar` to version 1.5.1 (build 6137 of 2026-11-12) from
  eQ-3. A call from `HmIP-RF` to a registered XML-RPC client now times out
  after `Legacy.Client.Socket.Timeout` (10 s if unset) instead of waiting
  forever for a client that accepted the connection but never answers, and
  a client gets events only after its `listDevices` succeeded
  ([homematicip#16](https://github.com/homematicip/OpenCCU-Base/pull/16),
  fixes [#192](https://github.com/OpenCCU/OpenCCU-Base/issues/192), @cn-eq3).
* `HmIP-RF` now retries the `listDevices` call to a newly registered client
  every 30 s while it fails, instead of dropping the client after the first
  failure, so a client whose callback port opens a moment late (e.g. the
  bridge of `VirtualDevices`, `HmIP-RF_java`, on a busy start) is no longer
  lost until the next restart
  ([homematicip#16](https://github.com/homematicip/OpenCCU-Base/pull/16),
  fixes [#180](https://github.com/OpenCCU/OpenCCU-Base/issues/180), @cn-eq3).
* a client that registers again on `HmIP-RF` with `init` (or deregisters)
  no longer leaves its HTTP client with one thread and two file descriptors
  behind. The same leak on `VirtualDevices` remains
  ([homematicip#16](https://github.com/homematicip/OpenCCU-Base/pull/16),
  [#36](https://github.com/OpenCCU/OpenCCU-Base/issues/36), @cn-eq3).
* update `HMIPServer.jar` to the build of 2026-10-08 (build 5936, version
  1.5.1-SNAPSHOT) from eQ-3: avoid a security counter update with invalid
  values. (@cn-eq3)
* lower `Legacy.Client.Connection.Timeout` in the `crRFD.conf` template
  from 600 s to 30 s. Since `HMIPServer.jar` 1.5.2 this value alone is the
  connect timeout of the calls to registered XML-RPC clients (before, the
  smaller of it and `Legacy.Client.Socket.Timeout`), so a call to a client
  on a host that does not answer at all (e.g. switched off) could wait up
  to 10 minutes for the connection; now it gives up after 30 s
  ([#361](https://github.com/OpenCCU/OpenCCU-Base/pull/361), @jens-maus).

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
