// SPDX-License-Identifier: Apache-2.0
#ifndef HSS_LED_CONTROLLER_H
#define HSS_LED_CONTROLLER_H
namespace LedController {
// run owns sysfs, timers and the control socket until termination. The optional
// callback starts the process-lifetime CCU status thread after signal setup.
int run(void (*startStatus)() = nullptr);
int client(int argc, char** argv);
void report(unsigned first, unsigned second, unsigned periodMs);
bool statusActive();
void configureTestPaths(); // environment overrides exist only in test builds
}
#endif
