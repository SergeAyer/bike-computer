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
 * @file gear_system_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Gear System header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "USBSerial.h"
#include "mbed.h"

namespace static_scheduling {

class GearSystemDevice {
   public:
    // constructor used for simulating the device with a thread
    GearSystemDevice();

    // method called for updating the bike system
    int getCurrentGear();

    // constants
    static constexpr uint8_t kMinGear = 1;
    static constexpr uint8_t kMaxGear = 9;

   private:
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

    // methods
    void read();

    // data members
    uint8_t _currentGear = kMinGear;
    Thread _thread;
};

}  // namespace static_scheduling
