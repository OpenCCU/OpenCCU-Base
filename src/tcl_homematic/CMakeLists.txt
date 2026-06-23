add_custom_target(tcl-homematic-assets
  COMMAND sh -c "cd '${CMAKE_CURRENT_SOURCE_DIR}' && ./create_homematic_tcl.tcl"
  COMMAND sh -c "cd '${CMAKE_CURRENT_SOURCE_DIR}' && ./stripcomments.tcl homematic.tcl > homematic_nocomments.tcl"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/lib/tcl8.2/homematic"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/homematic_nocomments.tcl" "${ROOTFS_DIR}/lib/tcl8.2/homematic/homematic.tcl"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/pkgIndex.tcl" "${ROOTFS_DIR}/lib/tcl8.2/homematic/pkgIndex.tcl"
  COMMAND "${CMAKE_COMMAND}" -E rm -f "${CMAKE_CURRENT_SOURCE_DIR}/homematic.tcl" "${CMAKE_CURRENT_SOURCE_DIR}/homematic_nocomments.tcl"
  VERBATIM
)
