option(HSS_LED_STATUS_MONITOR "Include the CCU status monitor" ON)
if(NOT HSS_LED_STATUS_MONITOR)
  add_executable(hss_led LedOnlyMain.cpp LedController.cpp)
  target_compile_features(hss_led PRIVATE cxx_std_11)
  target_compile_options(hss_led PRIVATE -Wall -Wextra)
  target_compile_definitions(hss_led PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
  target_link_libraries(hss_led PRIVATE pthread)
else()
add_executable(hss_led
  hss_led.cpp
  led.cpp
  LedController.cpp
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
target_compile_definitions(hss_led PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64 PLATFORM_CCU3)
target_compile_options(hss_led PRIVATE -Wall)
target_include_directories(hss_led PRIVATE
  "${CMAKE_SOURCE_DIR}/src/libelvutils"
  "${CMAKE_SOURCE_DIR}/src/libxmlparser"
  "${CMAKE_SOURCE_DIR}/src/libXmlRpc/src"
)
target_link_libraries(hss_led PRIVATE XmlRpc elvutils xmlparser pthread)
endif()
set_property(TARGET hss_led PROPERTY OPENCCU_RUNTIME_ALIASES hss_ledctl)
openccu_stage_target(hss_led bin)

option(HSS_LED_BUILD_TESTS "Build LED controller regression tests" OFF)
if(HSS_LED_BUILD_TESTS)
  add_executable(hss_led_state_test tests/LedControllerTest.cpp)
  target_compile_features(hss_led_state_test PRIVATE cxx_std_11)
  target_compile_options(hss_led_state_test PRIVATE -Wall -Wextra -Werror)
  target_compile_definitions(hss_led_state_test PRIVATE LED_TEST_BUILD)
  target_link_libraries(hss_led_state_test PRIVATE pthread)
  add_test(NAME hss_led_state COMMAND hss_led_state_test)
endif()
