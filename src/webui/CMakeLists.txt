add_subdirectory(www_source)
add_subdirectory(scripts)

add_custom_target(webui-assets
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/www"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/www" "${ROOTFS_DIR}/www"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/rega/www" "${ROOTFS_DIR}/www/rega"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink /tmp/EULA.de "${ROOTFS_DIR}/www/rega/EULA.de"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink /tmp/EULA.en "${ROOTFS_DIR}/www/rega/EULA.en"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink /etc/config/addons/www "${ROOTFS_DIR}/www/addons"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink tcl/extern/cgi.tcl "${ROOTFS_DIR}/www/cgi.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink tcl/eq3_old/common.tcl "${ROOTFS_DIR}/www/common.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink tcl/eq3_old/once.tcl "${ROOTFS_DIR}/www/once.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink tcl/eq3_old/session.tcl "${ROOTFS_DIR}/www/session.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink tcl/eq3_old/user.tcl "${ROOTFS_DIR}/www/user.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../../tcl/eq3/rega.tcl "${ROOTFS_DIR}/www/api/eq3/rega.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../../tcl/eq3/session.tcl "${ROOTFS_DIR}/www/api/eq3/session.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../../tcl/eq3/rega.tcl "${ROOTFS_DIR}/www/pda/eq3/rega.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../../tcl/eq3/session.tcl "${ROOTFS_DIR}/www/pda/eq3/session.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/extern/cgi.tcl "${ROOTFS_DIR}/www/config/cgi.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/eq3_old/common.tcl "${ROOTFS_DIR}/www/config/common.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/eq3_old/once.tcl "${ROOTFS_DIR}/www/config/once.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/eq3_old/session.tcl "${ROOTFS_DIR}/www/config/session.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../../tcl/extern/cgi.tcl "${ROOTFS_DIR}/www/config/display/cgi.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../../tcl/eq3_old/common.tcl "${ROOTFS_DIR}/www/config/display/common.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../../tcl/eq3_old/once.tcl "${ROOTFS_DIR}/www/config/display/once.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/eq3_old/user.tcl "${ROOTFS_DIR}/www/config/user.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/extern/cgi.tcl "${ROOTFS_DIR}/www/tools/cgi.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/eq3_old/common.tcl "${ROOTFS_DIR}/www/tools/common.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/eq3_old/once.tcl "${ROOTFS_DIR}/www/tools/once.tcl"
  COMMAND "${CMAKE_COMMAND}" -E create_symlink ../tcl/eq3_old/session.tcl "${ROOTFS_DIR}/www/tools/session.tcl"
  DEPENDS webui-www-source-assets webui-scripts-assets
  VERBATIM
)
