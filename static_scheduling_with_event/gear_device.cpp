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

#include "gear_device.hpp"

// from disco_h747i/wrappers
#include <chrono>

#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "GearDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling_with_event {

GearDevice::GearDevice() {
    disco::Joystick::getInstance().setUpCallback(callback(this, &GearDevice::onUp));
    disco::Joystick::getInstance().setDownCallback(callback(this, &GearDevice::onDown));
}

uint8_t GearDevice::getCurrentGear() { return core_util_atomic_load_u8(&_currentGear); }

void GearDevice::onUp() {
    if (_currentGear < bike_computer::kMaxGear) {
        core_util_atomic_incr_u8(&_currentGear, 1);
    }
}

void GearDevice::onDown() {
    if (_currentGear > bike_computer::kMinGear) {
        core_util_atomic_decr_u8(&_currentGear, 1);
    }
}

uint8_t GearDevice::getCurrentGearSize() const {
    // simulate task computation by waiting for the required task run time
    // wait_us(kTaskRunTime.count());
    uint8_t currentGear = core_util_atomic_load_u8(&_currentGear);
    return bike_computer::kMaxGearSize - currentGear;
}

}  // namespace static_scheduling_with_event
