// Copyright 2022 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file reset_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief ResetDevice implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "static_scheduling/reset_device.hpp"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
#define POLARITY_PRESSED 1
#endif

namespace static_scheduling {

ResetDevice::ResetDevice(Timer& timer) : _resetButton(PUSH_BUTTON), _timer(timer) {
    // register a callback for computing the response time
    _resetButton.fall(callback(this, &ResetDevice::onFall));
}

void ResetDevice::onFall() { _fallTime = _timer.elapsed_time(); }

const std::chrono::microseconds& ResetDevice::getFallTime() { return _fallTime; }

bool ResetDevice::checkReset() {
    // simulate task computation by waiting for the required task run time
    wait_us(kTaskRunTime.count());

    return _resetButton.read() == POLARITY_PRESSED;
}

}  // namespace static_scheduling
