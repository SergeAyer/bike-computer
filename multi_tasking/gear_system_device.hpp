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
 * @brief Gear system device header file
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

// from disco_h747i/wrappers
#include "joystick.hpp"

namespace multi_tasking {

class GearSystemDevice {
   public:
    // constructor used for simulating the device with a thread
    using GearSystemCallback = mbed::Callback<void(uint8_t)>;
    explicit GearSystemDevice(GearSystemCallback cb);

   private:
    void onJoystickUp();
    void onJoystickDown();

    // constants
    static constexpr uint8_t kMinGear = 1;
    static constexpr uint8_t kMaxNbrOfGears = 10;

    // data members
    GearSystemCallback _callback = nullptr;
    uint8_t _currentGear         = kMinGear;
};

}  // namespace multi_tasking
