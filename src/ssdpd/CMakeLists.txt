add_executable(ssdpd
  ssdpd.cpp
)

target_compile_features(ssdpd PRIVATE cxx_std_11)
target_compile_definitions(ssdpd PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(ssdpd PRIVATE -Wall)
target_link_libraries(ssdpd PRIVATE pthread)

openccu_stage_target(ssdpd bin)
