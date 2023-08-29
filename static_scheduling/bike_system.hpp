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
 * @brief Bike System header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "common/sensor_device.hpp"
#include "common/speedometer.hpp"
#include "display_device.hpp"
#include "gear_device.hpp"
#include "pedal_device.hpp"
#include "reset_device.hpp"
#include "utils/task_logger.hpp"

namespace static_scheduling {

class BikeSystem {
   public:
    // constructor
    BikeSystem();

    // method called in main() for starting the system
    void start();

   private:
    // private methods
    // called from the start() method super-loop for executing each task
    uint8_t readCurrentGear();
    uint8_t readCurrentGearSize();
    std::chrono::milliseconds readCurrentPedalRotationTime();
    float readSpeed();
    float readDistance();
    float readTemperature();
    void checkAndPerformReset();
    void updateGearOnDisplay(uint8_t gear);
    void updateSpeedOnDisplay(float speed);
    void updateDistanceOnDisplay(float distance);
    void updateTemperatureOnDisplay(float temperature);
    void computeSpeed();

    // timer instance used for loggint task time and used by ResetDevice
    Timer _timer;
    // data member that represents the device for manipulating the gear
    GearDevice _gearDevice;
    // data member that represents the device for manipulating the pedal rotation
    // speed/time
    PedalDevice _pedalDevice;
    // data member that represents the device used for resetting
    ResetDevice _resetDevice;
    // data member that represents the device display
    DisplayDevice _displayDevice;
    // data member that represents the device for counting wheel rotations
    bike_system::Speedometer _speedometer;
    // data member that represents the sensor device
    bike_system::SensorDevice _sensorDevice;

    // used for logging task info
    utils::TaskLogger _taskLogger;
};

}  // namespace static_scheduling
