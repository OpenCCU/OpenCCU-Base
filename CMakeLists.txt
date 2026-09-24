cmake_minimum_required(VERSION 3.20)

project(OpenCCUBase LANGUAGES C CXX)

include(CTest)

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
option(BUILD_DEFAULT_COMPONENTS "Default for individually selectable components" ON)
option(BUILD_TCL_MODULES "Default for Tcl extension components" ${BUILD_DEFAULT_COMPONENTS})
option(BUILD_WEBUI_AND_DEVICETYPES "Default for generated asset components" ${BUILD_DEFAULT_COMPONENTS})
option(HAS_USB_SUPPORT "Enable USB support in rfd" OFF)

set(OPENCCU_TCLSH_EXECUTABLE "tclsh" CACHE FILEPATH
    "Tcl interpreter used to generate WebUI assets")
set(OPENCCU_PYTHON_EXECUTABLE "python3" CACHE FILEPATH
    "Python interpreter used to convert generated WebUI assets")

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

  set_property(TARGET "${target_name}" PROPERTY OPENCCU_STAGE_DIR "${dest_subdir}")
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

include(cmake/Components.cmake)

message(STATUS "OpenCCU platform           : ${TARGET_PLATFORM}")
message(STATUS "OpenCCU staging rootfs dir : ${ROOTFS_DIR}")
message(STATUS "OpenCCU deploy-to-repo     : ${DEPLOY_TO_REPO}")
