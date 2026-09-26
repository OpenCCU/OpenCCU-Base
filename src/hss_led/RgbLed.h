// SPDX-License-Identifier: Apache-2.0
#ifndef HSS_RGB_LED_H
#define HSS_RGB_LED_H
#include "LedController.h"
// CCU status calculation and output live in one process. The controller
// consumes the most recent report without blocking the status thread.
class RgbLed {
public:
  void set(unsigned first, unsigned second, unsigned periodMs) {
    LedController::report(first, second, periodMs);
  }
};
#endif
