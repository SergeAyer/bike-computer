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
 * @file bike_system.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike System header file
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "bike_display.hpp"
#include "gear_system_device.hpp"
#include "memory_logger.hpp"
#include "reset_device.hpp"
#include "wheel_counter_device.hpp"

namespace multi_tasking {

class BikeSystem {
   public:
    // constructor
    BikeSystem();

    // method called in main() for starting the system
    void start();

   private:
    // private methods
    void setNewGear(uint8_t newGear);
    void updateCurrentGear(uint8_t newGear);
    void performReset();
    void processData();

    // called as reset handler
    void setReset();
    std::chrono::microseconds _resetTime;

    // memory logger
    MemoryLogger _memoryLogger;

    // data members used for exchanging information among threads
    CountQueue _countQueue;
    ProcessedMail _processedMail;

    // thread used for processing data
    Thread _processingThread;

    // EventQueue used for serving deferred ISRs
    EventQueue _eventQueueForISRs;

    // timer instance used for loggint task time and used by ResetDevice
    Timer _timer;
    // data member that represents the device for manipulating the gear
    GearSystemDevice _gearSystemDevice;
    // data member that represents the device for counting wheel rotations
    WheelCounterDevice _wheelCounterDevice;
    // data member that represents the device used for resetting
    ResetDevice _resetDevice;
    // data member that represents the device display
    BikeDisplay _bikeDisplay;
    // total rotation count
    volatile uint32_t _totalRotationCount = 0;
};

}  // namespace multi_tasking
