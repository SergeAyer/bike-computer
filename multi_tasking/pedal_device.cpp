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
 * @brief Pedal Device implementation (multi-tasking)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "pedal_device.hpp"

// from disco_h747i/wrappers
#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "PedalDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

PedalDevice::PedalDevice(EventQueue& eventQueue,
                         mbed::Callback<void(const std::chrono::milliseconds&)> cb)
    : _eventQueue(eventQueue), _cb(cb) {
    disco::Joystick::getInstance().setLeftCallback(callback(this, &PedalDevice::onLeft));
    disco::Joystick::getInstance().setRightCallback(
        callback(this, &PedalDevice::onRight));
    postEvent();
}

void PedalDevice::onLeft() {
    if (_currentStep < kNbrOfSteps) {
        _currentStep++;
        postEvent();
    }
}

void PedalDevice::onRight() {
    if (_currentStep > 0) {
        _currentStep--;
        postEvent();
    }
}

void PedalDevice::postEvent() {
    Event<void(const std::chrono::milliseconds&)> newRotationTimeEvent(&_eventQueue, _cb);
    _currentRotationTime = bike_computer::kMinPedalRotationTime +
                           _currentStep * bike_computer::kDeltaPedalRotationTime;
    newRotationTimeEvent.post(_currentRotationTime);
}

}  // namespace multi_tasking
