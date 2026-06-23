add_executable(eq3configd
  main.cpp
  src/networkconfig.cpp
  src/udpframe.cpp
)

target_compile_features(eq3configd PRIVATE cxx_std_11)
target_compile_definitions(eq3configd PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(eq3configd PRIVATE -Wall)
target_include_directories(eq3configd PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
)
target_link_libraries(eq3configd PRIVATE pthread crypto elvutils)

openccu_stage_target(eq3configd bin)
