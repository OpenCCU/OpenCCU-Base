/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "InfoLed.h"
#include "UdpCannel.h"
#include "LedController.h"
#include "LedCli.h"
#include <ConsoleLogger.h>
#include <SyslogLogger.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>

static void monitorStatus() {
  // S06 has initialized the user configuration at the former hss_led start
  // point. Do not cache rfd/LAN-gateway settings earlier than that.
  while (access("/var/status/hssLedReady", F_OK) != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  if (access("/usr/local/HMLGW", F_OK) == 0) return;
  InfoLed info;
  UdpCannel udp;
  std::string message;
  while (true) {
    if (udp.RefreshConnection()) break;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  while (true) {
    if (udp.ReceiveMessage(message) > 0 && !info.checkMessage(message))
      LOG(Logger::LOG_ERROR, "Cannot parse LED status message");
    if (LedController::statusActive()) info.updateLedState();
  }
}
static void startStatus() {
  std::thread([] {
    try { monitorStatus(); }
    catch (const std::exception& e) {
      std::cerr << "hss_led status: " << e.what() << '\n';
      std::_Exit(1); // let the supervisor restart the complete owner
    }
  }).detach();
}
int main(int argc, char** argv) {
  try {
    LedController::configureTestPaths();
    std::string program(argv[0]);
    program = program.substr(program.find_last_of('/') + 1);
    // Clients never construct InfoLed, bind UDP or start another controller.
    if (program == "hss_ledctl") return LedController::client(argc, argv);
    if (argc > 1 && std::string(argv[1]) == "--led-control")
      return LedController::client(argc - 1, argv + 1);
    bool ledOnly = access("/usr/local/HMLGW", F_OK) == 0;
    bool console = false;
    int level = static_cast<int>(Logger::LOG_INFO);
    for (int i = 1; i < argc; ++i) {
      std::string arg(argv[i]);
      if (arg == "--help" || arg == "-h") { LedCli::daemonHelp(std::cout); return 0; }
      if (arg == "--version" || arg == "-V") { LedCli::version(std::cout, "hss_led"); return 0; }
      if (arg == "--led-only") ledOnly = true;
      else if (arg == "-c") console = true;
      else if (arg == "-l" && i + 1 < argc) {
        std::string value(argv[++i]);
        if (value.size() != 1 || value[0] < '0' || value[0] > '6') {
          std::cerr << "Invalid log level; use -l 0..6.\n";
          return 2;
        }
        level = value[0] - '0';
      } else {
        std::cerr << "Invalid arguments; use hss_led --help.\n";
        return 2;
      }
    }
    logger = console ? static_cast<Logger*>(new ConsoleLogger()) : new SyslogLogger();
    logger->SetLevel(static_cast<Logger::LogLevel>(level));
    int result = LedController::run(ledOnly ? nullptr : startStatus);
    // The status thread may be blocked in a library query. State changes were
    // persisted before acknowledgment and run() has closed its descriptors.
    // Terminate the process without destructing shared globals under that thread.
    std::_Exit(result);
  } catch (const std::exception& e) {
    std::cerr << "hss_led: " << e.what() << '\n';
    std::_Exit(2);
  }
}
