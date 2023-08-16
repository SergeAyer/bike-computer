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
 * @file wheel_counter_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Wheel counter device header file
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "mbed.h"
#include "multi_tasking/data_types.hpp"

namespace multi_tasking {

class WheelCounterDevice {
   public:
    explicit WheelCounterDevice(CountQueue& countQueue);  // NOLINT(runtime/references)

    // the wheel counter device runs its own thread
    void start();

    // constant defining the Wheel circumference (in meters)
    static constexpr uint32_t kWheelCircumference = 2;

   private:
    // definition of wheel rotation time
    static constexpr std::chrono::milliseconds kWheelRotationTime = 200ms;
    static constexpr uint32_t kNbrOfRotationsForPut               = 5;

    // data members
    LowPowerTicker _ticker;
    uint32_t _rotationCount = 0;
    CountQueue& _countQueue;

    // private methods
    void turn();
};

}  // namespace multi_tasking
