// SPDX-License-Identifier: Apache-2.0
#ifndef OPENCCU_LED_CLI_H
#define OPENCCU_LED_CLI_H
#include <ostream>

namespace LedCli {
static const unsigned slowMs = 500;
static const unsigned fastMs = 100;

inline void version(std::ostream& out, const char* program, bool recovery = false) {
  out << program << " 2.3 (built " << __DATE__ << " " << __TIME__ << ")";
  if (recovery) out << " [recovery]";
  out << '\n';
}

inline void daemonHelp(std::ostream& out, bool recovery = false) {
  version(out, "hss_led", recovery);
  out << (recovery ? "LED controller for recovery.\n" : "CCU status monitor and LED controller.\n")
      << "\nUsage: hss_led [OPTIONS]\n"
         "       hss_led --led-control COMMAND...\n\n"
         "  -h, --help       Show this help and exit.\n"
         "  -V, --version    Show version and build timestamp, then exit.\n"
         "  --led-only       Run LED control without the CCU status monitor.\n";
  if (!recovery) out << "  -c               Log to the console instead of syslog.\n";
  out << "  -l LEVEL         Logging level from 0 to 6:\n"
         "                   0=all, 1=debug, 2=info, 3=notice,\n"
         "                   4=warning, 5=error, 6=fatal.\n";
  out << (recovery ? "                   Accepted for shared init scripts; recovery errors use stderr.\n"
                   : "                   Default: 2 (info).\n");
  out << "  --led-control    Run a client command instead of starting a daemon.\n\n"
         "The daemon stays in the foreground; the init script backgrounds it.\n"
         "Use hss_ledctl --help for LED targets, colors and blink commands.\n";
}
}
#endif
