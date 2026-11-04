# libXmlRpc ChangeLog

Changes of `libXmlRpc`, the XML-RPC and BIN-RPC library of rfd, hs485d,
hss_led, SetInterfaceClock, tclrpc and ReGaHss. Newest version first.

Changes not yet released are collected under `Unreleased` and get the
version and date with the next version bump (`XMLRPC_VERSION_NUMBER` in
`src/xmlrpc_version.h`).

## Unreleased

* add `xmlrpc_version()`, which returns the version of the loaded library
  (`XMLRPC_VERSION` still reads "XMLRPC++ 0.12").
  ([#294](https://github.com/OpenCCU/OpenCCU-Base/pull/294), @jens-maus)
* fix the RPC server no longer answering, or the process aborting, once a
  socket got a file descriptor of 1024 or more: the dispatcher waits with
  `poll()` instead of `select()`. ([#285](https://github.com/OpenCCU/OpenCCU-Base/pull/285), fixes [#178](https://github.com/OpenCCU/OpenCCU-Base/issues/178),
  @jens-maus)
* an interrupted `select()` (e.g. by a stop signal) is no longer logged as an
  error.
  ([#281](https://github.com/OpenCCU/OpenCCU-Base/pull/281), @jens-maus)
* fix an RPC method reading freed memory when a call has fewer arguments than
  the method reads: room for 8 more arguments is reserved before every call,
  so missing arguments can no longer move earlier ones.
  ([#271](https://github.com/OpenCCU/OpenCCU-Base/pull/271), fixes [#167](https://github.com/OpenCCU/OpenCCU-Base/issues/167),
  @jens-maus)

## 0.12 (2026-10-13)

* fix a crash (SIGSEGV) of the server on a deeply nested value, reachable
  with a single unauthenticated request: arrays and structs are parsed up to
  64 levels deep, a deeper value is rejected.
  ([#189](https://github.com/OpenCCU/OpenCCU-Base/pull/189), @jens-maus)
* fix out-of-bounds reads of the BIN-RPC deserializer on invalid length
  fields: such a request is rejected instead of crashing the server.
  ([#140](https://github.com/OpenCCU/OpenCCU-Base/pull/140), @jens-maus)

## 0.11 (2026-10-07)

* fix a crash (SIGSEGV) of the server on a `system.multicall` nested in
  another one: a nested `system.multicall` is rejected with a fault, as the
  specification requires.
  ([#138](https://github.com/OpenCCU/OpenCCU-Base/pull/138), @jens-maus)

## 0.10 (2026-09-07)

* fix a crash of the RPC client when a reconnect failed: the connection was
  removed twice from the dispatcher.
  ([#27](https://github.com/OpenCCU/OpenCCU-Base/pull/27), fixes [OpenCCU/OpenCCU#4138](https://github.com/OpenCCU/OpenCCU/issues/4138),
  @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
