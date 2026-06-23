add_executable(crypttool
  crypttool.cpp
  crypttoolconfig.cpp
)

target_compile_features(crypttool PRIVATE cxx_std_11)
target_compile_definitions(crypttool PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(crypttool PRIVATE -Wall)
target_include_directories(crypttool PRIVATE "${CMAKE_SOURCE_DIR}/src/libelvutils")
target_link_libraries(crypttool PRIVATE crypto pthread elvutils)

openccu_stage_target(crypttool bin)

add_custom_command(TARGET crypttool POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${ROOTFS_DIR}/etc/config_templates"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/config.txt"
          "${ROOTFS_DIR}/etc/config_templates/crypttool.cfg"
  VERBATIM
)

if(DEPLOY_TO_REPO)
  add_custom_command(TARGET crypttool POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DEPLOY_ROOT}/etc/config_templates"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/config.txt"
            "${DEPLOY_ROOT}/etc/config_templates/crypttool.cfg"
    VERBATIM
  )
endif()
