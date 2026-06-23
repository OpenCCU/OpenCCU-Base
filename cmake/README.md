# OpenCCU CMake Build Orchestration

This repository now provides a CMake-based build for `src/` modules.

## What it does

- Configures platform-aware builds via CMake presets/toolchain files.
- Builds native CMake modules directly across the `src/` tree.
- Stages build outputs into a rootfs-style install directory (`build/<preset>/rootfs`).
- Deploys binaries/libs into repository runtime paths:
  - `bin/<platform>/`
  - `lib/<platform>/`

## Configure and build

Native build:

```bash
cmake --preset x86_64-linux-gnu
cmake --build --preset x86_64-linux-gnu --target package
```

Top-level GNU Make wrapper:

```bash
# Build package for all supported platforms sequentially
make

# Copy staged outputs to top-level bin/<platform>, lib/<platform>, etc/
make install
```

Cross builds:

```bash
cmake --preset aarch64-linux-gnu
cmake --build --preset aarch64-linux-gnu --target package

cmake --preset arm-linux-gnueabihf
cmake --build --preset arm-linux-gnueabihf --target package

cmake --preset i686-linux-gnu
cmake --build --preset i686-linux-gnu --target package
```

## Main targets

- `core`: build and stage/deploy core daemons and libraries.
- `package`: `core` + copy `opt/` runtime tree into staged rootfs.
- Per-module targets use their module names directly (e.g. `rfd`, `libelvutils`, `hs485d`).

## Optional targets

- `BUILD_TCL_MODULES=ON`: also build `tclrega`, `tclrpc`, `tclticks`.
- `BUILD_WEBUI_AND_DEVICETYPES=ON`: run WebUI and devicetype asset generation into staging rootfs.

## Key cache variables

- `TARGET_PLATFORM`: target triple (`x86_64-linux-gnu`, `aarch64-linux-gnu`, `arm-linux-gnueabihf`, `i686-linux-gnu`).
- `CROSS_PREFIX`: compiler prefix (e.g. `aarch64-linux-gnu-`).
- `ROOTFS_DIR`: staging rootfs output directory.
- `DEPLOY_TO_REPO`: when `ON`, copy staged binaries/libs to `bin/<platform>` and `lib/<platform>`.
