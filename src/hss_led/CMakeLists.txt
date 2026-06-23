add_executable(hss_led
  hss_led.cpp
  led.cpp
  UdpCannel.cpp
  MessageParser.cpp
  AlarmMessage.cpp
  Info.cpp
  InfoLed.cpp
  InternetLed.cpp
  Network.cpp
  ServiceMessage.cpp
  UpdateAvailable.cpp
  RFLGWInfoLED.cpp
  hss_led_util.cpp
)
target_compile_features(hss_led PRIVATE cxx_std_11)
target_compile_definitions(hss_led PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(hss_led PRIVATE -Wall)
target_include_directories(hss_led PRIVATE
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
)
target_link_libraries(hss_led PRIVATE XmlRpc elvutils xmlparser pthread)
openccu_stage_target(hss_led bin)
