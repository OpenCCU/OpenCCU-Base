// SPDX-License-Identifier: Apache-2.0
// Integration fixture: a real controller with an intentionally stalled producer.
#include "../LedController.h"
#include <chrono>
#include <cstdlib>
#include <thread>
static void startStatus() {
  std::thread([] {
    LedController::report(4, 1, 70);
    std::this_thread::sleep_for(std::chrono::seconds(30));
  }).detach();
}
int main() {
  LedController::configureTestPaths();
  std::_Exit(LedController::run(startStatus));
}
