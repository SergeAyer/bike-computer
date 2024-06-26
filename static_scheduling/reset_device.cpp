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

#include "reset_device.hpp"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
static constexpr uint8_t kPolarityPressed = 1;
#endif

namespace static_scheduling {

// definition of task execution time
static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

ResetDevice::ResetDevice(Timer& timer) : _resetButton(PUSH_BUTTON), _timer(timer) {
    // register a callback for computing the response time
    _resetButton.rise(callback(this, &ResetDevice::onRise));
}

void ResetDevice::onRise() { _pressTime = _timer.elapsed_time(); }

std::chrono::microseconds ResetDevice::getPressTime() { return _pressTime; }

bool ResetDevice::checkReset() {
    bool reset                            = false;
    std::chrono::microseconds initialTime = _timer.elapsed_time();
    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    while (elapsedTime < kTaskRunTime) {
        if (!reset) {
            reset = _resetButton.read() == kPolarityPressed;
        }

        elapsedTime = _timer.elapsed_time() - initialTime;
    }
    return reset;
}

}  // namespace static_scheduling
