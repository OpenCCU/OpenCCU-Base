add_custom_target(devicetypes-assets
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/firmware/hs485types"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/firmware/rftypes/replaceMap"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/replaceMap/rfReplaceMap.xml" "${ROOTFS_DIR}/firmware/rftypes/replaceMap/rfReplaceMap.xml"
  COMMAND "${CMAKE_COMMAND}"
    "-DDEVICE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DDEVICE_OUTPUT_DIR=${ROOTFS_DIR}/firmware"
    "-DSTRIP_JAR=${CMAKE_SOURCE_DIR}/build-tools/bidcos-devicetype-strip.jar"
    "-DJAVA_EXECUTABLE=${OPENCCU_JAVA_EXECUTABLE}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/GenerateDeviceTypes.cmake"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/www/config"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/st_values.cgi" "${ROOTFS_DIR}/www/config/st_values.cgi"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/st_values.js" "${ROOTFS_DIR}/www/config/st_values.js"
  VERBATIM
)
