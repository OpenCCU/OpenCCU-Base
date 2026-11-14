#!/bin/bash
# Full release build of OpenCCU-Base for one platform, run as root inside a
# debian:bookworm-slim container with the repository mounted as the working
# directory (the same environment as the CI build):
#
#   docker run --rm --platform linux/amd64 --volume "$PWD:/workspace" \
#     --workdir /workspace debian:bookworm-slim \
#     scripts/release-build.sh <platform> [--assets]
#
# Builds every component from src/ and deploys the result into the working
# tree as the OpenCCU-Base releases ship it:
#
#   bin/<platform>/      programs (stripped, mode 0755)
#   lib/<platform>/      libraries and Tcl modules (stripped, mode 0644)
#   bin/hm_*             ReGa scripts (copies of src/webui/rega/bin/hm_*)
#   etc/config_templates/crypttool.cfg
#
# With --assets, the platform independent generated assets are deployed as
# well (only needed from one platform):
#
#   www/                 WebUI (generated from src/webui, incl. webui.js)
#   firmware/rftypes/, firmware/hs485types/
#                        device descriptions (generated from src/devicetypes)
#   usr/lib/tcl8.2/homematic/
#                        Tcl helpers (generated from src/tcl_homematic)
#
# The prebuilt ReGaHss binaries and lib/<platform>/tclticks.so, which is not
# built from src/, are left as they are.

set -euo pipefail

usage() {
  echo "usage: $0 <x86_64-linux-gnu|aarch64-linux-gnu|arm-linux-gnueabihf|i686-linux-gnu> [--assets]" >&2
  exit 2
}

[[ $# -ge 1 && $# -le 2 ]] || usage
platform=$1
assets=false
if [[ $# -eq 2 ]]; then
  [[ $2 == --assets ]] || usage
  assets=true
fi

package_arch=""
toolchain_packages=(build-essential)
strip_bin="strip"
case "${platform}" in
  x86_64-linux-gnu)
    multiarch_triplet=x86_64-linux-gnu
    ;;
  aarch64-linux-gnu)
    package_arch=arm64
    multiarch_triplet=aarch64-linux-gnu
    toolchain_packages=(gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
      libc6-dev-arm64-cross binutils-aarch64-linux-gnu)
    strip_bin=aarch64-linux-gnu-strip
    ;;
  arm-linux-gnueabihf)
    package_arch=armhf
    multiarch_triplet=arm-linux-gnueabihf
    toolchain_packages=(gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
      libc6-dev-armhf-cross binutils-arm-linux-gnueabihf)
    strip_bin=arm-linux-gnueabihf-strip
    ;;
  i686-linux-gnu)
    package_arch=i386
    multiarch_triplet=i386-linux-gnu
    toolchain_packages=(gcc-i686-linux-gnu g++-i686-linux-gnu
      libc6-dev-i386-cross binutils-i686-linux-gnu)
    strip_bin=i686-linux-gnu-strip
    ;;
  *)
    usage
    ;;
esac

export DEBIAN_FRONTEND=noninteractive
export LANG=C.UTF-8

suffix=""
if [[ -n ${package_arch} ]]; then
  dpkg --add-architecture "${package_arch}"
  suffix=":${package_arch}"
fi
apt-get update
apt-get install --yes --no-install-recommends \
  ca-certificates cmake default-jre-headless file make pkg-config python3 \
  rsync tcl "libssl-dev${suffix}" "libusb-1.0-0-dev${suffix}" \
  "tcl-dev${suffix}" "${toolchain_packages[@]}"

if [[ -n ${package_arch} ]]; then
  export PKG_CONFIG_PATH=
  export PKG_CONFIG_LIBDIR="/usr/lib/${multiarch_triplet}/pkgconfig:/usr/share/pkgconfig"
fi
tcl_include=$(pkg-config --variable=includedir tcl)
tcl_libdir=$(pkg-config --variable=libdir tcl)
test -f "${tcl_include}/tcl.h"
test -f "${tcl_libdir}/libtcl.so"

build_dir="build/${platform}"
rm -rf "${build_dir}"

if [[ ${assets} == true ]]; then
  # The device descriptions in firmware/ are generator output only. The
  # build stages firmware/ before it generates them, so remove them first:
  # otherwise a device removed from src/devicetypes would be kept.
  rm -rf firmware/rftypes firmware/hs485types
fi

# DEPLOY_TO_REPO copies every built program and library into bin/<platform>/
# and lib/<platform>/, the ReGa scripts into bin/ and crypttool.cfg into etc/.
cmake --preset "${platform}" \
  -DDEPLOY_TO_REPO=ON \
  -DBUILD_DEFAULT_COMPONENTS=ON \
  -DBUILD_TCL_MODULES=ON \
  -DBUILD_WEBUI_AND_DEVICETYPES=ON \
  -DBUILD_COMPAT_LIBRARIES=ON \
  -DHAS_USB_SUPPORT=ON \
  -DTCL_INCLUDE_PATH="${tcl_include}" \
  -DTCL_LIBRARY="${tcl_libdir}/libtcl.so"
cmake --build --preset "${platform}" --target package --parallel "$(nproc)"

# runtime-files.txt lists the built programs and libraries ("bin/rfd",
# "lib/libXmlRpc.so", ...), including the symbolic links of alternate names.
manifest="${build_dir}/runtime-files.txt"
test -s "${manifest}"
while read -r file; do
  [[ -n ${file} ]] || continue
  case "${file}" in
    bin/*) deployed="bin/${platform}/${file#bin/}"; mode=0755 ;;
    lib/*) deployed="lib/${platform}/${file#lib/}"; mode=0644 ;;
    *) echo "unexpected runtime file: ${file}" >&2; exit 1 ;;
  esac
  if [[ -L ${deployed} ]]; then
    continue
  fi
  test -s "${deployed}"
  "${strip_bin}" --strip-unneeded "${deployed}"
  chmod "${mode}" "${deployed}"
  file "${deployed}"
done < "${manifest}"

if [[ ${assets} == true ]]; then
  rootfs="${build_dir}/rootfs"
  for generated in www/webui/webui.js www/webui/style.css \
      www/config/devdescr/DEVDB.tcl usr/lib/tcl8.2/homematic/homematic.tcl \
      firmware/rftypes firmware/hs485types; do
    test -e "${rootfs}/${generated}"
  done
  # mirror the generated trees, so files removed from src/ disappear as well
  rsync -a --delete "${rootfs}/www/" www/
  rsync -a --delete "${rootfs}/firmware/rftypes/" firmware/rftypes/
  rsync -a --delete "${rootfs}/firmware/hs485types/" firmware/hs485types/
  rsync -a --delete "${rootfs}/usr/lib/tcl8.2/homematic/" usr/lib/tcl8.2/homematic/
fi

rm -rf "${build_dir}"
