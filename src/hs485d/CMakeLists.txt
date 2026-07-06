add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated-hs485d-version.h"
  COMMAND "${CMAKE_COMMAND}" -E env sh "${CMAKE_CURRENT_SOURCE_DIR}/version.sh" > /tmp/openccu-hs485d-version.txt
  COMMAND "${CMAKE_COMMAND}" -DINPUT=/tmp/openccu-hs485d-version.txt "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/generated-hs485d-version.h" -DMACRO_NAME=HS485D_VERSION -P "${CMAKE_SOURCE_DIR}/cmake/WriteVersionHeader.cmake"
  VERBATIM
)
add_custom_target(hs485d-version-header DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/generated-hs485d-version.h")

add_executable(hs485d
  hs485d.cpp
  HS485Central.cpp
  HS485Channel.cpp
  HS485ChannelDescription.cpp
  HS485DeviceDescription.cpp
  HS485Device.cpp
  HS485EEpromCache.cpp
  HS485FirmwareManager.cpp
  HS485Frame.cpp
  HS485CommMessage.cpp
  HS485CommMessageCCU1.cpp
  HS485CommMessageDecoder.cpp
  HS485Controller.cpp
  HS485ControllerCCU1.cpp
  HS485LogicalInstance.cpp
  HS485LogicalTypeAddress.cpp
  HS485MaintenanceChannel.cpp
  HS485Manager.cpp
  HS485Paramset.cpp
  HS485PhysicalAddress.cpp
  HS485PhysicalDataInterfaceCentralCommand.cpp
  HS485PhysicalDataInterfaceCommand.cpp
  HS485PhysicalDataInterfaceEEprom.cpp
  HS485PhysicalDataInterfaceStore.cpp
  HS485SystemDescription.cpp
  HS485TypeConversionAddressArray.cpp
  XmlRpcMethods.cpp
  LGWPortWrapper.cpp
  HS485CommMessageLGW.cpp
  HS485ControllerLGW.cpp
  HMWLGWTransportFrame.cpp
  HMWLGWCommand.cpp
)
add_dependencies(hs485d hs485d-version-header)
target_compile_features(hs485d PRIVATE cxx_std_11)
target_compile_definitions(hs485d PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(hs485d PRIVATE -Wall)
target_include_directories(hs485d PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libhsscomm"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
  "${CMAKE_SOURCE_DIR}/src/libUnifiedLanComm/include"
  "${CMAKE_SOURCE_DIR}/src/libLanDeviceUtils/include"
  "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(hs485d PRIVATE pthread UnifiedLanComm LanDeviceUtils elvutils hsscomm xmlparser XmlRpc crypto m)
openccu_stage_target(hs485d bin)
