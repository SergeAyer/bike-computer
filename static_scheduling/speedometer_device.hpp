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
 * @file speedometer_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief WheelCounterDevice header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include <chrono>

#include "mbed.h"

namespace static_scheduling {

class SpeedometerDevice {
   public:
    // Number of rotation count to wait before the device can deliver a speed
    static constexpr uint32_t kInitialRotationCount = 10;
    // When compiling and linking with gcc, we get a link error when using static
    // constexpr. The error is related to template instantiation. definition of pedal
    // rotation initial time (corresponds to 80 turn / min)
    static const std::chrono::milliseconds kInitialPedalRotationTime;
    // definition of pedal minimal rotation time (corresponds to 160 turn / min)
    static const std::chrono::milliseconds kMinPedalRotationTime;
    // definition of pedal maximal rotation time (corresponds to 10 turn / min)
    static const std::chrono::milliseconds kMaxPedalRotationTime;
    // definition of pedal rotation time change upon acceleration/deceleration
    static const std::chrono::milliseconds kDeltaRotationTime;

    explicit SpeedometerDevice(Timer& timer);  // NOLINT(runtime/references)

    // methods used for setting/getting the current gear
    void setGear(uint8_t gear);

    // method called for getting the wheel circumference
    uint8_t getGearSize();
    float getWheelCircumference();
    float getTraySize();

    // method called for getting the current pedal rotation time
    std::chrono::milliseconds getCurrentPedalRotationTime();
    // method called for getting the current speed
    float getCurrentSpeed();
    // method called for getting the current distance
    float getDistance();

    // these methods are made public for testing purposes
    void increaseRotationSpeed();
    void decreaseRotationSpeed();

    // method called for resetting the counter
    void reset();

   private:
    // private methods
    void update();
    void computeSpeedAndDistance();

    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 400ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 200000us;

    // constants related to speed computation
    static constexpr float kWheelCircumference   = 2.1f;
    static constexpr uint8_t kTraySize           = 50;
    std::chrono::microseconds _lastTime          = std::chrono::microseconds::zero();
    std::chrono::milliseconds _pedalRotationTime = std::chrono::milliseconds::zero();

    // data members
    Timer& _timer;
    LowPowerTicker _ticker;
    float _currentSpeed  = 0.0f;
    float _totalDistance = 0.0f;
    // smallest gear corresponds to a gear size of 20
    // when the gear increases, the gear size descreases
    uint8_t _gear                         = 1;
    static constexpr uint8_t kMaxGearSize = 16;

    Thread _thread;
};

}  // namespace static_scheduling
