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
 * @file gear_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Gear Device implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "static_scheduling/gear_device.hpp"

// from disco_h747i/wrappers
#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "GearDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

// reading rate in milliseconds when running a separate thread
// The gear value is updated every second
static constexpr std::chrono::milliseconds kReadingRate = 1000ms;

GearDevice::GearDevice()
    : _thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "GearDeviceThread") {
    _thread.start(callback(this, &GearDevice::read));
}

void GearDevice::read() {
    while (true) {
        disco::Joystick::State joystickState = disco::Joystick::getInstance().getState();
        switch (joystickState) {
            case disco::Joystick::State::UpPressed:
                if (_currentGear < bike_system::kMaxGear) {
                    _currentGear++;
                }
                break;

            case disco::Joystick::State::DownPressed:
                if (_currentGear > bike_system::kMinGear) {
                    _currentGear--;
                }
                break;

            default:
                break;
        }
        ThisThread::sleep_for(kReadingRate);
    }
}

uint8_t GearDevice::getCurrentGear() {
    // simulate task computation by waiting for the required task run time
    // wait_us(kTaskRunTime.count());
    return _currentGear;
}

uint8_t GearDevice::getCurrentGearSize() {
    // simulate task computation by waiting for the required task run time
    // wait_us(kTaskRunTime.count());
    return bike_system::kMaxGearSize - _currentGear;
}

}  // namespace static_scheduling