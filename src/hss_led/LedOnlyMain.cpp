// SPDX-License-Identifier: Apache-2.0
#include "LedController.h"
#include "LedCli.h"
#include <iostream>
#include <string>
int main(int argc, char** argv) {
  try {
    LedController::configureTestPaths();
    std::string program(argv[0]);
    program = program.substr(program.find_last_of('/') + 1);
    if (program == "hss_ledctl") return LedController::client(argc, argv);
    if (argc > 1 && std::string(argv[1]) == "--led-control")
      return LedController::client(argc - 1, argv + 1);
    for (int i = 1; i < argc; ++i) {
      std::string arg(argv[i]);
      if (arg == "--help" || arg == "-h") { LedCli::daemonHelp(std::cout, true); return 0; }
      if (arg == "--version" || arg == "-V") { LedCli::version(std::cout, "hss_led", true); return 0; }
      if (arg == "--led-only") continue;
      // Accept the shared init script's logging option. Recovery has no CCU
      // status logger; controller failures still go to stderr.
      if (arg == "-l" && i + 1 < argc) {
        std::string level(argv[++i]);
        if (level.size() == 1 && level[0] >= '0' && level[0] <= '6') continue;
      }
      std::cerr << "Invalid arguments; use hss_led --help.\n";
      return 2;
    }
    return LedController::run();
  } catch (const std::exception& e) {
    std::cerr << "hss_led: " << e.what() << '\n';
    return 2;
  }
}
