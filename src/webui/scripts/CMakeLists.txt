add_custom_target(webui-scripts-assets
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/lib"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/bin"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/libfirewall.tcl" "${ROOTFS_DIR}/lib/libfirewall.tcl"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/setfirewall.tcl" "${ROOTFS_DIR}/bin/setfirewall.tcl"
  VERBATIM
)
