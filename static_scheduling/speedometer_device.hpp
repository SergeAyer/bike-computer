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
 * @file bike_system.cpp
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
    explicit SpeedometerDevice(Timer& timer);  // NOLINT(runtime/references)

    // method called for getting the current speed
    void setGear(uint8_t gear);
    float getCurrentSpeed();
    float getDistance();

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
    // definition of pedal rotation initial time (corresponds to 80 turn / min)
    static constexpr std::chrono::milliseconds kInitialPedalRotationTime = 750ms;
    // definition of pedal rotation minimal time (corresponds to 160 turn / min)
    static constexpr std::chrono::milliseconds kMinPedalRotationTime = 375ms;
    // definition of pedal rotation minimal time (corresponds to 10 turn / min)
    static constexpr std::chrono::milliseconds kMaxPedalRotationTime = 6000ms;
    // definition of pedal rotation time change upon acc/dec
    static constexpr std::chrono::milliseconds kDeltaRotationTime = 25ms;
    // constants related to speed computation
    static constexpr uint32_t kInitialRotationCount = 10;
    static constexpr float kWheelCircumference      = 2.1f;
    static constexpr uint8_t kTraySize              = 50;
    std::chrono::microseconds _lastTime             = std::chrono::microseconds::zero();
    std::chrono::milliseconds _pedalRotationTime    = kInitialPedalRotationTime;

    // data members
    Timer& _timer;
    LowPowerTicker _ticker;
    uint32_t _rotationCount = 0;
    float _currentSpeed     = 0.0f;
    float _totalDistance    = 0.0f;
    // smallest gear corresponds to a gear size of 20
    // when the gear increases, the gear size descreases
    uint8_t _gear                         = 1;
    static constexpr uint8_t kMaxGearSize = 21;

    Thread _thread;
};

}  // namespace static_scheduling
