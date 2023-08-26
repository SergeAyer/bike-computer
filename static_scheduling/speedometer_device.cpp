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
 * @file speedometer_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief WheelCounterDevice implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "static_scheduling/speedometer_device.hpp"

#include <ratio>

// from disco_h747i/wrappers
#include "joystick.hpp"
#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "SpeedometerDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

// constant definitions
const std::chrono::milliseconds SpeedometerDevice::kInitialPedalRotationTime = 750ms;
const std::chrono::milliseconds SpeedometerDevice::kMinPedalRotationTime     = 375ms;
const std::chrono::milliseconds SpeedometerDevice::kMaxPedalRotationTime     = 1500ms;
const std::chrono::milliseconds SpeedometerDevice::kDeltaRotationTime        = 25ms;

SpeedometerDevice::SpeedometerDevice(Timer& timer)
    : _pedalRotationTime(kInitialPedalRotationTime), _timer(timer) {
    _thread.start(callback(this, &SpeedometerDevice::update));
}

void SpeedometerDevice::setGear(uint8_t gear) { _gear = gear; }

uint8_t SpeedometerDevice::getGearSize() { return kMaxGearSize - _gear; }

float SpeedometerDevice::getWheelCircumference() { return kWheelCircumference; }

float SpeedometerDevice::getTraySize() { return kTraySize; }

std::chrono::milliseconds SpeedometerDevice::getCurrentPedalRotationTime() {
    return _pedalRotationTime;
}

float SpeedometerDevice::getCurrentSpeed() {
    // simulate task computation by waiting for the required task run time
    wait_us(kTaskRunTime.count());
    return _currentSpeed;
}

float SpeedometerDevice::getDistance() {
    // simulate task computation by waiting for the required task run time
    wait_us(kTaskRunTime.count());
    return _totalDistance;
}

void SpeedometerDevice::reset() {}

void SpeedometerDevice::update() {
    // start with an initial rotation count for proper initialization
    _lastTime = _timer.elapsed_time();
    ThisThread::sleep_for(_pedalRotationTime * kInitialRotationCount);

    while (true) {
        // compute speed and distance since last call
        computeSpeedAndDistance();

        // check whether speed has been updated
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

        // by sleeping for the rotation time, we simulate turns
        ThisThread::sleep_for(_pedalRotationTime);
    }
}

void SpeedometerDevice::increaseRotationSpeed() {
    if (_pedalRotationTime > kMinPedalRotationTime) {
        _pedalRotationTime -= kDeltaRotationTime;
    }
}

void SpeedometerDevice::decreaseRotationSpeed() {
    if (_pedalRotationTime < kMaxPedalRotationTime) {
        _pedalRotationTime += kDeltaRotationTime;
    }
}

void SpeedometerDevice::computeSpeedAndDistance() {
    // For computing the distance given a rear gear (braquet), one must divide the size of
    // the tray (plateau) by the size of the rear gear (pignon arrière), and then multiply
    // the result by the circumference of the wheel. Example: tray = 50, rear gear = 15.
    // Distance run with one pedal turn (wheel circumference = 2.10 m) = 50/15 * 2.1 m
    // = 6.99m If you ride at 80 pedal turns / min, you run a distance of 6.99 * 80 / min
    // ~= 560 m / min = 33.6 km/h

    // compute the elapsed time since last call
    std::chrono::microseconds time = _timer.elapsed_time();
    const auto elapsedTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(time - _lastTime);
    // compute the number of pedal rotations since last call
    float pedalRotations = static_cast<float>(elapsedTime.count()) /
                           static_cast<float>(_pedalRotationTime.count());

    // compute the distance per pedal rotation
    uint8_t gearSize = kMaxGearSize - _gear;
    // distance run in the elapsed time (in m)
    float distancePerPedalRotation =
        (static_cast<float>(kTraySize) / static_cast<float>(gearSize)) *
        kWheelCircumference;

    // update the total distance
    float distance = (distancePerPedalRotation * pedalRotations);
    _totalDistance += distance;

    // update the current speed
    // speed (m / ms) is converted to (km / h) by multiplying by 3'600'000 / 1000 = 3'600
    _currentSpeed = (distance * 3600.0f) / elapsedTime.count();
    tr_debug("Pedal rotations %f, distance %f, speed %f, elapsed time %" PRIu64 "",
             pedalRotations,
             distance,
             _currentSpeed,
             elapsedTime.count());

    // update _lastTime
    _lastTime = _timer.elapsed_time();
}

}  // namespace static_scheduling
