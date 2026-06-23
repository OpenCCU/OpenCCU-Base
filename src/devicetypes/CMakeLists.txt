add_custom_target(devicetypes-assets
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/firmware/hs485types"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/firmware/rftypes/replaceMap"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/replaceMap/rfReplaceMap.xml" "${ROOTFS_DIR}/firmware/rftypes/replaceMap/rfReplaceMap.xml"
  COMMAND sh -c "for f in \"${CMAKE_CURRENT_SOURCE_DIR}\"/rftypes/*.xml; do java -jar \"${CMAKE_SOURCE_DIR}/build-tools/bidcos-devicetype-strip.jar\" \"$f\" -o \"${ROOTFS_DIR}/firmware/rftypes/$(basename \"$f\")\"; done"
  COMMAND sh -c "for f in \"${CMAKE_CURRENT_SOURCE_DIR}\"/hs485types/*.xml; do java -jar \"${CMAKE_SOURCE_DIR}/build-tools/bidcos-devicetype-strip.jar\" \"$f\" -o \"${ROOTFS_DIR}/firmware/hs485types/$(basename \"$f\")\"; done"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/www/config"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/st_values.cgi" "${ROOTFS_DIR}/www/config/st_values.cgi"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/st_values.js" "${ROOTFS_DIR}/www/config/st_values.js"
  VERBATIM
)
