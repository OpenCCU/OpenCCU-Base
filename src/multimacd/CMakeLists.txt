file(GLOB_RECURSE MULTIMACD_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated-multimacd-version.h"
  COMMAND "${CMAKE_COMMAND}" -E env sh "${CMAKE_CURRENT_SOURCE_DIR}/version.sh" > /tmp/openccu-multimacd-version.txt
  COMMAND "${CMAKE_COMMAND}" -DINPUT=/tmp/openccu-multimacd-version.txt -DOUTPUT="${CMAKE_CURRENT_BINARY_DIR}/generated-multimacd-version.h" -DMACRO_NAME=MULTIMACD_VERSION -P "${CMAKE_SOURCE_DIR}/cmake/WriteVersionHeader.cmake"
  VERBATIM
)
add_custom_target(multimacd-version-header DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/generated-multimacd-version.h")

add_executable(multimacd ${MULTIMACD_SOURCES})
add_dependencies(multimacd multimacd-version-header)
target_compile_features(multimacd PRIVATE cxx_std_11)
target_compile_definitions(multimacd PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(multimacd PRIVATE -Wall)
target_include_directories(multimacd PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(multimacd PRIVATE pthread rt m crypto elvutils)
openccu_stage_target(multimacd bin)
