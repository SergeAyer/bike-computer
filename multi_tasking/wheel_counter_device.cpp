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
 * @file wheel_counter_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Wheel counter device implementation
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#include "multi_tasking/wheel_counter_device.hpp"

namespace multi_tasking {
    
static constexpr std::chrono::milliseconds kWheelRotationTime = 200ms;

WheelCounterDevice::WheelCounterDevice(CountQueue& countQueue)
    : _countQueue(countQueue) {}

void WheelCounterDevice::start() {
    // start a ticker for signaling a wheel rotation
    _ticker.attach(callback(this, &WheelCounterDevice::turn), kWheelRotationTime);
}

void WheelCounterDevice::turn() {
    // ISR context
    // increment rotation count
    _rotationCount++;
    // try to push the data to the queue, if successful reset the rotation count
    if (_rotationCount > kNbrOfRotationsForPut &&
        // NOLINTNEXTLINE(readability/casting)
        _countQueue.try_put((uint32_t*)_rotationCount)) {
        _rotationCount = 0;
    }
}

}  // namespace multi_tasking
