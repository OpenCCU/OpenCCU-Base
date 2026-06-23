add_executable(hs485dLoader
  hs485dLoader.cpp
)

target_compile_features(hs485dLoader PRIVATE cxx_std_11)
target_compile_definitions(hs485dLoader PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(hs485dLoader PRIVATE -Wall)
target_include_directories(hs485dLoader PRIVATE
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
)
target_link_libraries(hs485dLoader PRIVATE pthread m c elvutils xmlparser)

openccu_stage_target(hs485dLoader bin)
