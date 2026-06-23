cmake_minimum_required(VERSION 3.20)

project(OpenCCUBase LANGUAGES C CXX)

set(SUPPORTED_PLATFORMS
    aarch64-linux-gnu
    arm-linux-gnueabihf
    i686-linux-gnu
    x86_64-linux-gnu
)

set(TARGET_PLATFORM "x86_64-linux-gnu" CACHE STRING "OpenCCU target platform triple")
set_property(CACHE TARGET_PLATFORM PROPERTY STRINGS ${SUPPORTED_PLATFORMS})

if(NOT TARGET_PLATFORM IN_LIST SUPPORTED_PLATFORMS)
  message(FATAL_ERROR
    "Unsupported TARGET_PLATFORM='${TARGET_PLATFORM}'. Supported: ${SUPPORTED_PLATFORMS}")
endif()

set(CROSS_PREFIX "${TARGET_PLATFORM}-" CACHE STRING "Cross toolchain prefix (e.g. aarch64-linux-gnu-)")
set(ROOTFS_DIR "${CMAKE_BINARY_DIR}/rootfs" CACHE PATH "Staging rootfs directory")
set(DEPLOY_ROOT "${CMAKE_SOURCE_DIR}" CACHE PATH "Repository root for deployed artifacts")
option(DEPLOY_TO_REPO "Deploy staged artifacts into bin/lib/... under repository root" ON)
option(BUILD_TCL_MODULES "Build Tcl extension modules from src/tcl*" OFF)
option(BUILD_WEBUI_AND_DEVICETYPES "Generate/install webui + devicetype assets from src/" OFF)
option(HAS_USB_SUPPORT "Enable USB support in rfd" OFF)

add_custom_target(prepare-stage
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/bin"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/lib"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/www"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/etc"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/firmware"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/opt"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/usr"
  VERBATIM
)

function(openccu_stage_target target_name dest_subdir)
  if(dest_subdir STREQUAL "bin" OR dest_subdir STREQUAL "lib")
    set(_deploy_dir "${DEPLOY_ROOT}/${dest_subdir}/${TARGET_PLATFORM}")
  else()
    set(_deploy_dir "${DEPLOY_ROOT}/${dest_subdir}")
  endif()

  add_dependencies("${target_name}" prepare-stage)
  add_custom_command(TARGET "${target_name}" POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/${dest_subdir}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${target_name}>" "${ROOTFS_DIR}/${dest_subdir}/$<TARGET_FILE_NAME:${target_name}>"
    VERBATIM
  )

  if(DEPLOY_TO_REPO)
    add_custom_command(TARGET "${target_name}" POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${_deploy_dir}"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${target_name}>" "${_deploy_dir}/$<TARGET_FILE_NAME:${target_name}>"
      VERBATIM
    )
  endif()
endfunction()

add_subdirectory(src/libelvutils)
add_subdirectory(src/libxmlparser)
add_subdirectory(src/libXmlRpc)
add_subdirectory(src/libhsscomm)
add_subdirectory(src/libLanDeviceUtils)
add_subdirectory(src/libUnifiedLanComm)
add_subdirectory(src/libeq3config)
add_subdirectory(src/eq3configcmd)
add_subdirectory(src/eq3configd)
add_subdirectory(src/ssdpd)
add_subdirectory(src/crypttool)
add_subdirectory(src/multimacd)
add_subdirectory(src/hs485d)
add_subdirectory(src/hs485dLoader)
add_subdirectory(src/hss_led)
add_subdirectory(src/rfd)
add_subdirectory(src/bcm2835_raw_uart)

add_executable(SetInterfaceClock src/SetInterfaceClock/SetInterfaceClock.cpp)
target_compile_features(SetInterfaceClock PRIVATE cxx_std_11)
target_compile_definitions(SetInterfaceClock PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_include_directories(SetInterfaceClock PRIVATE
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
  "${CMAKE_SOURCE_DIR}/src/libhsscomm"
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
)
target_link_libraries(SetInterfaceClock PRIVATE XmlRpc hsscomm xmlparser elvutils pthread crypto)
openccu_stage_target(SetInterfaceClock bin)

if(BUILD_TCL_MODULES)
  add_subdirectory(src/tclrega)
  add_subdirectory(src/tclrpc)
  add_subdirectory(src/tclticks)
endif()

if(BUILD_WEBUI_AND_DEVICETYPES)
  add_subdirectory(src/devicetypes)
  add_subdirectory(src/webui)
  add_subdirectory(src/tcl_homematic)
endif()

add_custom_target(core
  DEPENDS
    elvutils
    xmlparser
    XmlRpc
    hsscomm
    LanDeviceUtils
    UnifiedLanComm
    eq3config
    eq3configd
    ssdpd
    crypttool
    multimacd
    hs485d
    hs485dLoader
    hss_led
    rfd
    eq3configcmd
    SetInterfaceClock
)

if(BUILD_TCL_MODULES)
  add_dependencies(core
    tclrega
    tclrpc
    tclticks
  )
endif()

if(BUILD_WEBUI_AND_DEVICETYPES)
  add_dependencies(core devicetypes-assets webui-assets tcl-homematic-assets)
endif()

add_custom_target(package
  DEPENDS core
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/opt"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/opt" "${ROOTFS_DIR}/opt"
  VERBATIM
)

message(STATUS "OpenCCU platform           : ${TARGET_PLATFORM}")
message(STATUS "OpenCCU cross prefix       : ${CROSS_PREFIX}")
message(STATUS "OpenCCU staging rootfs dir : ${ROOTFS_DIR}")
message(STATUS "OpenCCU deploy-to-repo     : ${DEPLOY_TO_REPO}")
