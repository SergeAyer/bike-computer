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
 * @file reset_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Reset device header file
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

namespace multi_tasking {

class ResetDevice {
   public:
    // constructor used for event-driven behavior
    using ResetDeviceCallback = mbed::Callback<void()>;
    explicit ResetDevice(ResetDeviceCallback cb);

   private:
    // data members
    // instance representing the reset button
    InterruptIn _resetButton;
};

}  // namespace multi_tasking
