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
 * @file gear_system_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Gear system device implementation
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#include "multi_tasking/gear_system_device.hpp"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "GearSystemDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

GearSystemDevice::GearSystemDevice(GearSystemCallback cb) {
    disco::Joystick::getInstance().setSelCallback(
        callback(this, &GearSystemDevice::onJoystickUp));
    disco::Joystick::getInstance().setDownCallback(
        callback(this, &GearSystemDevice::onJoystickDown));
    _callback = cb;
    //tr_debug("Callback set");
}

void GearSystemDevice::onJoystickUp() {
    if (_currentGear < kMaxNbrOfGears) {
        _currentGear++;
        if (_callback != nullptr) {
            _callback(_currentGear);
        }
    }
}

void GearSystemDevice::onJoystickDown() {
    if (_currentGear > kMinGear) {
        _currentGear--;
        if (_callback != nullptr) {
            _callback(_currentGear);
        }
    }
}


}  // namespace multi_tasking
