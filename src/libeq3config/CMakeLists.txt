add_library(eq3config SHARED
  CommandManager.cpp
  Command.cpp
  Commands.cpp
  HelpSystem.cpp
  CCU2CommControllerMod/CCU2CommControllerMod.cpp
  CCU2CommControllerMod/CCU2CoprocessorCommandMod.cpp
  CCU2CommControllerMod/CCU2LGWCommControllerMod.cpp
  CCU2CommControllerMod/CCU2PortWrapperMod.cpp
  CCU2CommControllerMod/CCU2SerialFrameMod.cpp
  CCU2CommControllerMod/CCU2SerialPortCommControllerMod.cpp
  CCU2CommControllerMod/CCU2SerialPortWrapperLinuxMod.cpp
  CCU2CommControllerMod/CCU2SerialPortWrapperMod.cpp
  CCU2CommControllerMod/FileIOMod.cpp
  CCU2CommControllerMod/HM2UtilsMod.cpp
  CCU2CommControllerMod/LGWPortWrapperMod.cpp
  netconfigcmd/netconfigcmd.cpp
  netconfigcmd/src/networkconfig.cpp
  netconfigcmd/src/udpframe.cpp
  update-lgw-firmware/update-lgw-firmware.cpp
  lgwallocation/lgwallocation.cpp
  update-coprocessor/update-coprocessor.cpp
  setlgwkey/setlgwkey.cpp
  rfd-interface-copy/rfd-interface-copy.cpp
  read-default-rf-address/ReadDefaultRFAddress.cpp
  wait-for-file/wait-for-file.cpp
  test-coprocessor/test-coprocessor.cpp
  md5/md5.cpp
  md5/MD5Module.cpp
)
target_compile_features(eq3config PRIVATE cxx_std_11)
target_compile_definitions(eq3config PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(eq3config PRIVATE -Wall -fPIC)
target_include_directories(eq3config PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/CCU2CommControllerMod"
  "${CMAKE_CURRENT_SOURCE_DIR}/netconfigcmd"
  "${CMAKE_CURRENT_SOURCE_DIR}/netconfigcmd/src/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/update-lgw-firmware"
  "${CMAKE_CURRENT_SOURCE_DIR}/update-coprocessor"
  "${CMAKE_CURRENT_SOURCE_DIR}/setlgwkey"
  "${CMAKE_CURRENT_SOURCE_DIR}/rfd-interface-copy"
  "${CMAKE_CURRENT_SOURCE_DIR}/read-default-rf-address"
  "${CMAKE_CURRENT_SOURCE_DIR}/wait-for-file"
  "${CMAKE_CURRENT_SOURCE_DIR}/test-coprocessor"
  "${CMAKE_CURRENT_SOURCE_DIR}/md5"
  "${CMAKE_CURRENT_SOURCE_DIR}/lgwallocation"
  "${CMAKE_SOURCE_DIR}/src/libLanDeviceUtils/include"
  "${CMAKE_SOURCE_DIR}/src/libUnifiedLanComm/include"
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
)
target_link_libraries(eq3config PRIVATE pthread rt crypto LanDeviceUtils UnifiedLanComm elvutils)
set_target_properties(eq3config PROPERTIES OUTPUT_NAME eq3config)
openccu_stage_target(eq3config lib)
