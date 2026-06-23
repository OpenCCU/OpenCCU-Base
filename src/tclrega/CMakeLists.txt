find_package(TCL QUIET)
if(NOT TCL_FOUND)
  message(FATAL_ERROR "BUILD_TCL_MODULES=ON requires Tcl development files.")
endif()

add_library(tclrega MODULE tclrega.cpp)
set_target_properties(tclrega PROPERTIES PREFIX "" OUTPUT_NAME "tclrega")
target_compile_options(tclrega PRIVATE -fPIC -O2 -W -Wall -Wno-write-strings)
target_include_directories(tclrega PRIVATE
  "${TCL_INCLUDE_PATH}"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
)
target_link_libraries(tclrega PRIVATE "${TCL_LIBRARY}" xmlparser stdc++)
openccu_stage_target(tclrega lib)
