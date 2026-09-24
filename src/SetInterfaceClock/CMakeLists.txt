add_executable(SetInterfaceClock SetInterfaceClock.cpp)
target_compile_features(SetInterfaceClock PRIVATE cxx_std_11)
target_compile_definitions(SetInterfaceClock PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_include_directories(SetInterfaceClock PRIVATE
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
  "${CMAKE_SOURCE_DIR}/src/libhsscomm"
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
)
target_link_libraries(SetInterfaceClock PRIVATE XmlRpc hsscomm xmlparser elvutils pthread crypto)
openccu_stage_target(SetInterfaceClock bin)
