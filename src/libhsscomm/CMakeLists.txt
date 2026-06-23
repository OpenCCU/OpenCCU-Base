add_library(hsscomm SHARED
  CommController.cpp
  CommMessage.cpp
  CommMessageQueue.cpp
  PortWrapper.cpp
  SocketPortWrapper.cpp
  UnixSerialPortWrapper.cpp
  AsyncXmlRpcSender.cpp
  ValueStore.cpp
  FrameDescription.cpp
  LogicalInstance.cpp
  type_registry.cpp
  HSSDescription.cpp
  HSSIntegerValueMap.cpp
  HSSLogicalType.cpp
  HSSLogicalTypeAction.cpp
  HSSLogicalTypeBoolean.cpp
  HSSLogicalTypeFloat.cpp
  HSSLogicalTypeInteger.cpp
  HSSLogicalTypeOption.cpp
  HSSLogicalTypeString.cpp
  HSSManager.cpp
  HSSParameter.cpp
  HSSParamset.cpp
  HSSPhysicalDataInterface.cpp
  HSSPhysicalDataInterfaceInternal.cpp
  HSSPhysicalType.cpp
  HSSPhysicalTypeArray.cpp
  HSSPhysicalTypeBoolean.cpp
  HSSPhysicalTypeInteger.cpp
  HSSPhysicalTypeString.cpp
  HSSTypeConversionActionInteger.cpp
  HSSTypeConversionBooleanInteger.cpp
  HSSTypeConversionFloatConfigtime.cpp
  HSSTypeConversionFloatInteger.cpp
  HSSTypeConversionStringUnsignedInteger.cpp
  HSSTypeConversion.cpp
  HSSTypeConversionAnyKeyCounter.cpp
  HSSTypeConversionSameKeyCounter.cpp
  HSSTypeConversionIdentity.cpp
  HSSTypeConversionIntegerIntegerMap.cpp
  HSSTypeConversionIntegerIntegerScale.cpp
  HSSTypeConversionIntegerTinyfloat.cpp
  HSSTypeConversionToggle.cpp
  HSSTypeConversionRc19Display.cpp
  HSSTypeConversionCfm.cpp
  HSSTypeConversionCCRTDNParty.cpp
  HSSTypeConversionBlindTest.cpp
  HSSTypeConversionHexStringToByteArray.cpp
  HSSTypeConversionInt4ToSignedIntX.cpp
  HSSTypeConversionFloatUInt8String.cpp
  MetadataStore.cpp
  StructuredFrame.cpp
  HSSXmlRpcEventDispatcher.cpp
  HSSXmlRpcEventMessage.cpp
)
target_compile_features(hsscomm PRIVATE cxx_std_11)
target_compile_definitions(hsscomm PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(hsscomm PRIVATE -Wall -fPIC)
target_include_directories(hsscomm PRIVATE
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
)
target_link_libraries(hsscomm PRIVATE pthread elvutils xmlparser XmlRpc)
set_target_properties(hsscomm PROPERTIES OUTPUT_NAME hsscomm)
openccu_stage_target(hsscomm lib)
