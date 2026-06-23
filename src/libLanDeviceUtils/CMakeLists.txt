add_library(LanDeviceUtils SHARED
  src/Constants.cpp
  src/EQ3ConfigProtocol.cpp
  src/IPConfiguration.cpp
  src/LanDevice.cpp
  src/LanDeviceUtils.cpp
  src/LanifCfgProtocol.cpp
  src/NetworkInterfaceHelper.cpp
  src/Protocol.cpp
  src/RuntimeIPConfiguration.cpp
  src/TestStatusConfiguration.cpp
  src/UDPDatagramSender.cpp
  src/UtilityDeviceConfiguration.cpp
  src/UtilityFirmwareUpdate.cpp
  src/UtilityNetworkConfiguration.cpp
  src/UtilitySearchDevices.cpp
  src/UtilitySendCommand.cpp
  src/TCPEncryption.cpp
)
target_compile_features(LanDeviceUtils PRIVATE cxx_std_11)
target_compile_definitions(LanDeviceUtils PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(LanDeviceUtils PRIVATE -Wall -fPIC)
target_include_directories(LanDeviceUtils PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(LanDeviceUtils PRIVATE "${CMAKE_SOURCE_DIR}/src/libelvutils")
target_link_libraries(LanDeviceUtils PRIVATE pthread crypto elvutils)
set_target_properties(LanDeviceUtils PROPERTIES OUTPUT_NAME LanDeviceUtils)
openccu_stage_target(LanDeviceUtils lib)
