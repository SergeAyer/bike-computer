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
 * @file pedal_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Pedal System header file (multi-tasking)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include <chrono>

#include "constants.hpp"
#include "mbed.h"

namespace multi_tasking {

class PedalDevice {
   public:
    PedalDevice(EventQueue& eventQueue,  // NOLINT(runtime/references)
                mbed::Callback<void(const std::chrono::milliseconds&)> cb);

    // make the class non copyable
    PedalDevice(PedalDevice&)            = delete;
    PedalDevice& operator=(PedalDevice&) = delete;

   private:
    // private methods
    void onLeft();
    void onRight();
    void postEvent();

    // data members
    static constexpr uint32_t kNbrOfSteps = static_cast<uint32_t>(
        (bike_computer::kMaxPedalRotationTime - bike_computer::kMinPedalRotationTime)
            .count() /
        bike_computer::kDeltaPedalRotationTime.count());
    volatile uint32_t _currentStep = static_cast<uint32_t>(
        (bike_computer::kInitialPedalRotationTime - bike_computer::kMinPedalRotationTime)
            .count() /
        bike_computer::kDeltaPedalRotationTime.count());
    std::chrono::milliseconds _currentRotationTime;
    // reference to EventQueue used for posting events upon rotation speed change
    EventQueue& _eventQueue;
    mbed::Callback<void(const std::chrono::milliseconds&)> _cb;
};

}  // namespace multi_tasking
