add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/generated-rfd-version.h"
  COMMAND "${CMAKE_COMMAND}" -E env sh "${CMAKE_CURRENT_SOURCE_DIR}/version.sh" > /tmp/openccu-rfd-version.txt
  COMMAND "${CMAKE_COMMAND}" -DINPUT=/tmp/openccu-rfd-version.txt "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/generated-rfd-version.h" -DMACRO_NAME=RFD_VERSION -P "${CMAKE_SOURCE_DIR}/cmake/WriteVersionHeader.cmake"
  VERBATIM
)
add_custom_target(rfd-version-header DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/generated-rfd-version.h")

set(RFD_SOURCES
  rfd.cpp
  BidcosFrame.cpp
  BidcosFrameDecoder.cpp
  BidcosFrameDetermineValue.cpp
  BidcosFrameEnterConfig.cpp
  BidcosFrameLinkPeerReq.cpp
  BidcosFrameParamReq.cpp
  BidcosInterface.cpp
  BidcosInterfaceConcentrator.cpp
  BidcosInterfaceConnection.cpp
  BidcosInterfaceMessage.cpp
  BidcosLanConnection.cpp
  BidcosLanInterface.cpp
  BidcosRemoteInterface.cpp
  RFCentral.cpp
  RFChannel.cpp
  RFChannelDescription.cpp
  RFCommMessage.cpp
  RFCommMessageAESKey.cpp
  RFCommMessageDecoder.cpp
  RFConfigData.cpp
  RFController.cpp
  RFDevice.cpp
  RFDeviceDescription.cpp
  RFLoggingManager.cpp
  RFLogicalInstance.cpp
  RFMaintenanceChannel.cpp
  RFManager.cpp
  RFParamset.cpp
  RFPhysicalDataInterfaceCommand.cpp
  RFPhysicalDataInterfaceCentralCommand.cpp
  RFPhysicalDataInterfaceConfig.cpp
  RFPhysicalDataInterfaceConfigString.cpp
  RFPhysicalDataInterfaceStore.cpp
  RFPhysicalDataInterfaceMultiframeCommand.cpp
  RFSystemDescription.cpp
  RFTeam.cpp
  XmlRpcMethods.cpp
  hm2/CCU2PortWrapper.cpp
  hm2/LGWPortWrapper.cpp
  hm2/FileIO.cpp
  hm2/HM2Utils.cpp
  hm2/CCU2SerialFrame.cpp
  hm2/CCU2SerialPortWrapper.cpp
  hm2/CCU2SerialPortWrapperLinux.cpp
  hm2/CCU2BidcosRemoteInterface.cpp
  hm2/CCU2CoprocessorCommand.cpp
  hm2/CCU2CommController.cpp
  hm2/CCU2SerialPortCommController.cpp
  hm2/CCU2LGWCommController.cpp
  RFFirmwareManager.cpp
  RFReplaceMap.cpp
  BidcosFrameStartBootloader.cpp
  JSONObject.cpp
)

if(HAS_USB_SUPPORT)
  list(APPEND RFD_SOURCES
    hid_linux/HIDDevice.cpp
    hid_linux/HIDReportDescriptor.cpp
    BidcosUsbInterface.cpp
    BidcosUsbConnection.cpp
  )
endif()

add_executable(rfd ${RFD_SOURCES})
add_dependencies(rfd rfd-version-header)
target_compile_features(rfd PRIVATE cxx_std_11)
target_compile_definitions(rfd PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64 PRODUCTION=1)
target_compile_options(rfd PRIVATE -Wall)
target_include_directories(rfd PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/hm2"
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libhsscomm"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
  "${CMAKE_SOURCE_DIR}/src/libLanDeviceUtils/include"
  "${CMAKE_SOURCE_DIR}/src/libUnifiedLanComm/include"
  "${CMAKE_CURRENT_BINARY_DIR}"
)
if(HAS_USB_SUPPORT)
  target_compile_definitions(rfd PRIVATE HAS_USB_SUPPORT)
  target_include_directories(rfd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/hid_linux")
endif()
target_link_libraries(rfd PRIVATE pthread elvutils hsscomm xmlparser XmlRpc LanDeviceUtils UnifiedLanComm crypto m)
if(HAS_USB_SUPPORT)
  target_link_libraries(rfd PRIVATE usb-1.0)
endif()
openccu_stage_target(rfd bin)
