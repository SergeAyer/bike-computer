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

#include "pedal_device.hpp"

// from disco_h747i/wrappers
#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "PedalDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling_with_event {

PedalDevice::PedalDevice() {
    disco::Joystick::getInstance().setLeftCallback(callback(this, &PedalDevice::onLeft));
    disco::Joystick::getInstance().setRightCallback(
        callback(this, &PedalDevice::onRight));
}

void PedalDevice::onLeft() { decreaseRotationSpeed(); }

void PedalDevice::onRight() { increaseRotationSpeed(); }

std::chrono::milliseconds PedalDevice::getCurrentRotationTime() {
    uint32_t currentStep = core_util_atomic_load_u32(&_currentStep);
    return bike_computer::kMinPedalRotationTime +
           currentStep * bike_computer::kDeltaPedalRotationTime;
}

void PedalDevice::increaseRotationSpeed() {
    uint32_t currentStep = core_util_atomic_load_u32(&_currentStep);
    if (currentStep > 0) {
        core_util_atomic_decr_u32(&_currentStep, 1);
    }
}

void PedalDevice::decreaseRotationSpeed() {
    uint32_t currentStep = core_util_atomic_load_u32(&_currentStep);
    if (currentStep < kNbrOfSteps) {
        core_util_atomic_incr_u32(&_currentStep, 1);
    }
}

}  // namespace static_scheduling_with_event
