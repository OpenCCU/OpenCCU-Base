add_subdirectory(www_source)
add_subdirectory(scripts)

add_custom_target(webui-assets
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/www"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/www" "${ROOTFS_DIR}/www"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink /var/EULA.de "${ROOTFS_DIR}/www/rega/EULA.de"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink /var/EULA.en "${ROOTFS_DIR}/www/rega/EULA.en"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink /var/EULA.tr "${ROOTFS_DIR}/www/rega/EULA.tr"
  DEPENDS webui-www-source-assets webui-scripts-assets
  VERBATIM
)
