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

## Selecting components

Each native tool has a `BUILD_<NAME>` option, enabled by default:

- `BUILD_SETINTERFACECLOCK`, `BUILD_CRYPTTOOL`, `BUILD_EQ3CONFIGCMD`
- `BUILD_EQ3CONFIGD`, `BUILD_SSDPD`, `BUILD_MULTIMACD`, `BUILD_RFD`
- `BUILD_HS485D`, `BUILD_HS485DLOADER`, `BUILD_HSS_LED`
- `BUILD_TCLREGA`, `BUILD_TCLRPC`

Runtime data has separate options: `BUILD_WEBUI`, `BUILD_DEVICETYPES`,
`BUILD_TCL_HOMEMATIC`, `BUILD_HMSERVER`, `BUILD_HMIP_TOOLS`, `BUILD_FIRMWARE`,
`BUILD_HM_SCRIPTS` and `BUILD_REGAHSS`. These also default to enabled. ReGaHss and
Java programs are prebuilt assets, not applications compiled by this project.
WebUI selection also enables its device descriptions, Tcl helpers, Tcl extensions
and ReGaHss. ReGaHss selection enables its Tcl extension.

Use a fresh build directory and `BUILD_DEFAULT_COMPONENTS=OFF` to start with an
empty selection. For example, build and install only the configuration tool:

```bash
cmake -S . -B build/config-tool -DBUILD_DEFAULT_COMPONENTS=OFF \
  -DBUILD_EQ3CONFIGCMD=ON -DDEPLOY_TO_REPO=OFF
cmake --build build/config-tool --target package
DESTDIR="$PWD/image" cmake --install build/config-tool --prefix /
```

CMake builds and installs the transitive internal libraries linked by the selected
targets. The configuration tool above includes `eq3config`, `LanDeviceUtils`,
`UnifiedLanComm` and `elvutils`; an `ssdpd`-only build needs none of those libraries.
System libraries such as OpenSSL and Tcl must be supplied by the toolchain or
package manager. No separate recovery mode is needed.

`BUILD_TCL_MODULES` and `BUILD_WEBUI_AND_DEVICETYPES` remain available as defaults
for their individual options. Per-component cache values take precedence over
these defaults. Kernel targets have `BUILD_BCM2835_RAW_UART` and
`BUILD_EQ3_CHAR_LOOP` configuration options and remain explicit build targets.

## Targets and installation

- `core` and `package`: build and stage the selected components.
- `compat-libraries`: explicitly build the legacy `xmlparser` and `XmlRpc` pair.
  Set `BUILD_COMPAT_LIBRARIES=ON` to include them in `core` and installation.
- Individual native targets remain directly buildable when enabled.
- `cmake --install`: install the configured selection. Component `runtime`
  contains native programs and internal libraries; `assets` contains runtime data.
- `runtime-files.txt` in the build directory lists the native installation set.

Use the install rules rather than copying the entire staging directory: a reused
staging directory can still contain outputs from a previous selection. The GNU
Make wrapper accepts `CMAKE_OPTIONS` and copies a fresh selected installation.
When producing a new image, start with an empty destination; installation does
not uninstall files from an older image.

Run the dependency and installation regression checks with:

```bash
python3 tests/test-components.py -v
```

## Key cache variables

- `TARGET_PLATFORM`: target triple (`x86_64-linux-gnu`, `aarch64-linux-gnu`, `arm-linux-gnueabihf`, `i686-linux-gnu`).
- `CROSS_PREFIX`: compiler prefix (e.g. `aarch64-linux-gnu-`).
- `ROOTFS_DIR`: staging rootfs output directory.
- `DEPLOY_TO_REPO`: when `ON`, copy staged binaries/libs to `bin/<platform>` and `lib/<platform>`.
