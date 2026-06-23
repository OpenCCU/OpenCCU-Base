add_library(UnifiedLanComm SHARED
  src/LanConnection.cpp
  src/CommonConversion.cpp
  src/UnifiedLanCommController.cpp
  src/UnifiedLanProtocolMessage.cpp
  src/UnifiedLanProtocolTypeConverter.cpp
  src/LanConnectionListener.cpp
)
target_compile_features(UnifiedLanComm PRIVATE cxx_std_11)
target_compile_definitions(UnifiedLanComm PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(UnifiedLanComm PRIVATE -Wall -fPIC)
target_include_directories(UnifiedLanComm PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(UnifiedLanComm PRIVATE
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libLanDeviceUtils/include"
)
target_link_libraries(UnifiedLanComm PRIVATE pthread LanDeviceUtils elvutils)
set_target_properties(UnifiedLanComm PROPERTIES OUTPUT_NAME UnifiedLanComm)
openccu_stage_target(UnifiedLanComm lib)
