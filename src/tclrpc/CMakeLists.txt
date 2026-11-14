find_package(TCL QUIET)
if(NOT TCL_FOUND)
  message(FATAL_ERROR "BUILD_TCL_MODULES=ON requires Tcl development files.")
endif()

add_library(tclrpc MODULE tclrpc.cpp)
set_target_properties(tclrpc PROPERTIES PREFIX "" OUTPUT_NAME "tclrpc")
target_compile_options(tclrpc PRIVATE -fPIC -O2 -W -Wall -Wno-write-strings)
target_include_directories(tclrpc PRIVATE
  "${TCL_INCLUDE_PATH}"
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
)
target_link_libraries(tclrpc PRIVATE "${TCL_LIBRARY}" XmlRpc stdc++)
openccu_stage_target(tclrpc lib)
