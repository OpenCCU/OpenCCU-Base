# Runtime data is selected independently of native programs. Prebuilt ReGaHss
# and Java archives are staged as assets; they are not advertised as source builds.
foreach(component WEBUI DEVICETYPES TCL_HOMEMATIC)
  option(BUILD_${component} "Generate ${component} assets" ${BUILD_WEBUI_AND_DEVICETYPES})
endforeach()
foreach(component HMSERVER HMIP_TOOLS FIRMWARE HM_SCRIPTS REGAHSS)
  option(BUILD_${component} "Stage ${component} runtime assets" ${BUILD_DEFAULT_COMPONENTS})
endforeach()
if(BUILD_WEBUI)
  # st_values.* and the Tcl helpers are part of the WebUI runtime.
  set(BUILD_DEVICETYPES ON)
  set(BUILD_TCL_HOMEMATIC ON)
  set(BUILD_REGAHSS ON)
  set(BUILD_TCLRPC ON)
endif()

if(BUILD_REGAHSS)
  set(BUILD_TCLREGA ON)
endif()

# Configure internal libraries once, but let CMake's actual link graph decide
# which ones to build. Unused libraries do not enter ALL or the install set.
foreach(library elvutils xmlparser XmlRpc hsscomm LanDeviceUtils UnifiedLanComm eq3config)
  add_subdirectory(src/lib${library} EXCLUDE_FROM_ALL)
endforeach()

add_custom_target(core ALL)
set(openccu_roots)
foreach(program SetInterfaceClock crypttool eq3configcmd eq3configd hs485d
                hs485dLoader hss_led multimacd rfd ssdpd)
  string(TOUPPER "${program}" option_name)
  option(BUILD_${option_name} "Build ${program}" ${BUILD_DEFAULT_COMPONENTS})
  if(BUILD_${option_name})
    add_subdirectory(src/${program})
    list(APPEND openccu_roots ${program})
  endif()
endforeach()

foreach(module tclrega tclrpc)
  string(TOUPPER "${module}" option_name)
  option(BUILD_${option_name} "Build ${module}" ${BUILD_TCL_MODULES})
  if(BUILD_${option_name})
    add_subdirectory(src/${module})
    list(APPEND openccu_roots ${module})
  endif()
endforeach()

option(BUILD_COMPAT_LIBRARIES "Include the legacy xmlparser/XmlRpc runtime" OFF)
add_custom_target(compat-libraries DEPENDS xmlparser XmlRpc)
if(BUILD_COMPAT_LIBRARIES)
  list(APPEND openccu_roots xmlparser XmlRpc)
endif()

# Kernel modules keep their explicit, non-ALL build targets.
foreach(module bcm2835_raw_uart eq3_char_loop)
  string(TOUPPER "${module}" option_name)
  option(BUILD_${option_name} "Configure the ${module} kernel module target" ${BUILD_DEFAULT_COMPONENTS})
  if(BUILD_${option_name})
    add_subdirectory(src/${module})
  endif()
endforeach()

# Follow the same LINK_LIBRARIES graph used by the linker. No second library
# dependency table in downstream packaging is needed. Imported/system libraries
# are supplied by the toolchain/package manager rather than installed here.
function(openccu_collect_runtime target)
  get_property(collected GLOBAL PROPERTY OPENCCU_RUNTIME_TARGETS)
  if(NOT TARGET "${target}" OR "${target}" IN_LIST collected)
    return()
  endif()
  get_target_property(destination "${target}" OPENCCU_STAGE_DIR)
  if(NOT destination)
    return()
  endif()
  set_property(GLOBAL APPEND PROPERTY OPENCCU_RUNTIME_TARGETS "${target}")
  get_target_property(dependencies "${target}" LINK_LIBRARIES)
  foreach(dependency IN LISTS dependencies)
    openccu_collect_runtime("${dependency}")
  endforeach()
endfunction()
foreach(target IN LISTS openccu_roots)
  add_dependencies(core ${target})
  openccu_collect_runtime(${target})
endforeach()
get_property(runtime_targets GLOBAL PROPERTY OPENCCU_RUNTIME_TARGETS)
set(runtime_manifest "")
foreach(target IN LISTS runtime_targets)
  get_target_property(destination ${target} OPENCCU_STAGE_DIR)
  string(APPEND runtime_manifest "${destination}/$<TARGET_FILE_NAME:${target}>\n")
  # Also re-stage up-to-date outputs after the staging directory was cleaned.
  add_custom_command(TARGET core POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/${destination}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:${target}>" "${ROOTFS_DIR}/${destination}/$<TARGET_FILE_NAME:${target}>"
    VERBATIM)
  # A program providing alternate invocations declares them on its target.
  get_property(aliases TARGET ${target} PROPERTY OPENCCU_RUNTIME_ALIASES)
  foreach(alias IN LISTS aliases)
    string(APPEND runtime_manifest "${destination}/${alias}\n")
    # Generate the actual link at build time, when target names are resolved.
    add_custom_command(TARGET core POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E create_symlink "$<TARGET_FILE_NAME:${target}>"
              "${ROOTFS_DIR}/${destination}/${alias}"
      VERBATIM)
    if(DEPLOY_TO_REPO)
      add_custom_command(TARGET core POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${DEPLOY_ROOT}/${destination}/${TARGET_PLATFORM}"
        COMMAND "${CMAKE_COMMAND}" -E create_symlink "$<TARGET_FILE_NAME:${target}>"
                "${DEPLOY_ROOT}/${destination}/${TARGET_PLATFORM}/${alias}"
        VERBATIM)
    endif()
    install(CODE "
      file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${destination}\")
      file(CREATE_LINK \"$<TARGET_FILE_NAME:${target}>\"
        \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${destination}/${alias}\" SYMBOLIC)
      " COMPONENT runtime)
  endforeach()
  install(TARGETS ${target}
    RUNTIME DESTINATION bin COMPONENT runtime
    LIBRARY DESTINATION lib COMPONENT runtime)
endforeach()
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/runtime-files.txt" CONTENT "${runtime_manifest}")

foreach(pair "WEBUI|webui|webui-assets" "DEVICETYPES|devicetypes|devicetypes-assets"
             "TCL_HOMEMATIC|tcl_homematic|tcl-homematic-assets")
  string(REPLACE "|" ";" fields "${pair}")
  list(GET fields 0 component)
  list(GET fields 1 directory)
  list(GET fields 2 target)
  if(BUILD_${component})
    add_subdirectory(src/${directory})
    add_dependencies(core ${target})
  endif()
endforeach()

function(openccu_asset_directory component source destination)
  if(BUILD_${component})
    add_custom_target(${component}-assets
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/${destination}"
      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/${source}" "${ROOTFS_DIR}/${destination}"
      VERBATIM)
    add_dependencies(core ${component}-assets)
    install(DIRECTORY "${ROOTFS_DIR}/${destination}/" DESTINATION "${destination}" COMPONENT assets)
  endif()
endfunction()
openccu_asset_directory(HMSERVER opt/HMServer opt/HMServer)
openccu_asset_directory(HMIP_TOOLS opt/HmIP opt/HmIP)
openccu_asset_directory(FIRMWARE firmware firmware)
if(BUILD_FIRMWARE AND BUILD_DEVICETYPES)
  add_dependencies(devicetypes-assets FIRMWARE-assets)
endif()
if(BUILD_DEVICETYPES AND NOT BUILD_FIRMWARE)
  install(DIRECTORY "${ROOTFS_DIR}/firmware/" DESTINATION firmware COMPONENT assets)
endif()
if(BUILD_WEBUI)
  install(DIRECTORY "${ROOTFS_DIR}/www/" DESTINATION www COMPONENT assets)
endif()
if(BUILD_TCL_HOMEMATIC)
  install(DIRECTORY "${ROOTFS_DIR}/usr/" DESTINATION usr COMPONENT assets)
endif()
set(scripts)
if(BUILD_HM_SCRIPTS)
  list(APPEND scripts hm_autoconf hm_deldev hm_startup)
endif()
if(BUILD_REGAHSS)
  list(APPEND scripts ${TARGET_PLATFORM}/ReGaHss)
endif()
foreach(script IN LISTS scripts)
  get_filename_component(name "${script}" NAME)
  add_custom_command(TARGET core POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/bin"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bin/${script}" "${ROOTFS_DIR}/bin/${name}"
    VERBATIM)
  install(PROGRAMS "${CMAKE_SOURCE_DIR}/bin/${script}" DESTINATION bin COMPONENT assets)
endforeach()

if(BUILD_CRYPTTOOL)
  install(FILES src/crypttool/config.txt DESTINATION etc/config_templates
    RENAME crypttool.cfg COMPONENT assets)
endif()
if(BUILD_HMSERVER)
  install(FILES etc/config_templates/crRFD.conf etc/config_templates/log4j2.xml
    DESTINATION etc/config_templates COMPONENT assets)
endif()

add_custom_target(package DEPENDS core)
if(BUILD_TESTING)
  add_test(NAME bidcos-devicetype-strip
    COMMAND "${CMAKE_SOURCE_DIR}/tests/build-tools/test-bidcos-devicetype-strip.sh")
endif()
