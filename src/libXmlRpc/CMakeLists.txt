add_library(XmlRpc SHARED
  src/XmlRpcClient.cpp
  src/XmlRpcDispatch.cpp
  src/XmlRpcServer.cpp
  src/XmlRpcServerProxy.cpp
  src/XmlRpcServerConnection.cpp
  src/XmlRpcServerMethod.cpp
  src/XmlRpcSocket.cpp
  src/XmlRpcSource.cpp
  src/XmlRpcUtil.cpp
  src/XmlRpcValue.cpp
)
target_compile_features(XmlRpc PRIVATE cxx_std_11)
target_compile_definitions(XmlRpc PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(XmlRpc PRIVATE -Wall -fPIC)
target_include_directories(XmlRpc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_target_properties(XmlRpc PROPERTIES OUTPUT_NAME XmlRpc)
openccu_stage_target(XmlRpc lib)
