add_custom_target(webui-www-source-assets
  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}" ./check_CheckOnlyFiles.tcl
  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}" ./create_webui_js.tcl
  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}" ./utf82ansi.py webui.js
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/www/webui"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/webui.js" "${ROOTFS_DIR}/www/webui/webui.js"
  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}" ./create_style_css.tcl
  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}" ./utf82ansi.py style.css
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/style.css" "${ROOTFS_DIR}/www/webui/style.css"
  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}" ./create_devdb_tcl.tcl
  COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}" ./utf82ansi.py DEVDB.tcl
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/www/config/devdescr"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/DEVDB.tcl" "${ROOTFS_DIR}/www/config/devdescr/DEVDB.tcl"
  COMMAND "${CMAKE_COMMAND}" -E rm -f
          "${CMAKE_CURRENT_SOURCE_DIR}/webui.js"
          "${CMAKE_CURRENT_SOURCE_DIR}/style.css"
          "${CMAKE_CURRENT_SOURCE_DIR}/DEVDB.tcl"
          "${CMAKE_CURRENT_SOURCE_DIR}/webui_js.tmp"
          "${CMAKE_CURRENT_SOURCE_DIR}/style_css.tmp"
          "${CMAKE_CURRENT_SOURCE_DIR}/DEVDB_tcl.tmp"
          "${CMAKE_CURRENT_SOURCE_DIR}/devdescr.js"
          "${CMAKE_CURRENT_SOURCE_DIR}/st_values.js"
  VERBATIM
)
