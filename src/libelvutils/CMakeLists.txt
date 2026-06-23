add_library(elvutils SHARED
  Actor.cpp
  ActorMessage.cpp
  ConsoleLogger.cpp
  Crc16.cpp
  FileLogger.cpp
  Hexfile.cpp
  Logger.cpp
  md5.cpp
  MulticastSender.cpp
  OSCompatLinux.cpp
  PropertyMap.cpp
  PThreadCondition.cpp
  SyslogLogger.cpp
  TimeZoneInfo.cpp
  TimerQueue.cpp
  TimerTarget.cpp
  TimestampLinux.cpp
  unsigned_char_traits.cpp
  UpdateFile.cpp
  utils.cpp
)
target_compile_features(elvutils PRIVATE cxx_std_11)
target_compile_definitions(elvutils PRIVATE _TIME_BITS=64 _FILE_OFFSET_BITS=64)
target_compile_options(elvutils PRIVATE -Wall -fPIC)
target_link_libraries(elvutils PRIVATE crypto)
set_target_properties(elvutils PROPERTIES OUTPUT_NAME elvutils)
openccu_stage_target(elvutils lib)
