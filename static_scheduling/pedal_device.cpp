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
 * @file pedal_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Pedal Device implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "static_scheduling/pedal_device.hpp"

// from disco_h747i/wrappers
#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "PedalDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

// reading rate in milliseconds when running a separate thread
static constexpr std::chrono::milliseconds kReadingRate = 1000ms;

PedalDevice::PedalDevice() { _thread.start(callback(this, &PedalDevice::read)); }

void PedalDevice::read() {
    while (true) {
        // check whether rotation speed has been updated
        disco::Joystick::State joystickState = disco::Joystick::getInstance().getState();
        switch (joystickState) {
            case disco::Joystick::State::RightPressed:
                increaseRotationSpeed();
                break;

            case disco::Joystick::State::LeftPressed:
                decreaseRotationSpeed();
                break;

            default:
                break;
        }
        ThisThread::sleep_for(kReadingRate);
    }
}

std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    // simulate task computation by waiting for the required task run time
    // wait_us(kTaskRunTime.count());
    return _pedalRotationTime;
}

void PedalDevice::increaseRotationSpeed() {
    if (_pedalRotationTime > bike_system::kMinPedalRotationTime) {
        _pedalRotationTime -= bike_system::kDeltaPedalRotationTime;
    }
}

void PedalDevice::decreaseRotationSpeed() {
    if (_pedalRotationTime < bike_system::kMaxPedalRotationTime) {
        _pedalRotationTime += bike_system::kDeltaPedalRotationTime;
    }
}

}  // namespace static_scheduling
