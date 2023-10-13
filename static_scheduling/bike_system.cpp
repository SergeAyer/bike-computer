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
 * @brief Bike System implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "static_scheduling/bike_system.hpp"

#include <chrono>

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

static constexpr std::chrono::microseconds kSensorTaskRunTime = 1000ms;

BikeSystem::BikeSystem()
    : _gearDevice(_timer),
      _pedalDevice(_timer),
      _resetDevice(_timer),
      _speedometer(_timer) {}

void BikeSystem::start() {
    tr_info("Starting Super-Loop with no event handling");

    // start the timer
    _timer.start();

    // initialize the lcd display
    disco::ReturnCode rc = _displayDevice.init();
    if (rc != disco::ReturnCode::Ok) {
        tr_error("Failed to initialized the lcd display: %d", static_cast<int>(rc));
    }

    // initialize the sensor device
    bool present = _sensorDevice.init();
    if (!present) {
        tr_error("Sensor not present or initialization failed");
    }

    // enable/disable task logging
    _taskLogger.enable(true);

    while (true) {
        // register the time at the beginning of the cyclic schedule period
        auto startTime = _timer.elapsed_time();

        // gear task
        auto taskStartTime = _timer.elapsed_time();

        const auto gear     = _gearDevice.getCurrentGear();
        const auto gearSize = _gearDevice.getCurrentGearSize();

        _taskLogger.logPeriodAndExecutionTime(
            _timer, advembsof::TaskLogger::kGearTaskIndex, taskStartTime);

        // speed and distance task
        taskStartTime = _timer.elapsed_time();

        const auto pedalRotationTime = _pedalDevice.getCurrentRotationTime();
        _speedometer.setCurrentRotationTime(pedalRotationTime);
        _speedometer.setGearSize(gearSize);
        const auto speed    = _speedometer.getCurrentSpeed();
        const auto distance = _speedometer.getDistance();

        _taskLogger.logPeriodAndExecutionTime(
            _timer, advembsof::TaskLogger::kSpeedTaskIndex, taskStartTime);

        // temperature task
        taskStartTime = _timer.elapsed_time();

        const auto temperature = _sensorDevice.readTemperature();

        // simulate task computation by waiting for the required task run time
        wait_us(kSensorTaskRunTime.count());

        _taskLogger.logPeriodAndExecutionTime(
            _timer, advembsof::TaskLogger::kSensorTaskIndex, taskStartTime);

        // reset task
        taskStartTime = _timer.elapsed_time();

        if (_resetDevice.checkReset()) {
            tr_info("Reset task: response time is %" PRIu64 " usecs",
                    (_timer.elapsed_time() - _resetDevice.getFallTime()).count());
            _speedometer.reset();
        }

        _taskLogger.logPeriodAndExecutionTime(
            _timer, advembsof::TaskLogger::kResetTaskIndex, taskStartTime);

        // display task
        taskStartTime = _timer.elapsed_time();

        _displayDevice.displayGear(gear);
        _displayDevice.displaySpeed(speed);
        _displayDevice.displayDistance(distance);
        _displayDevice.displayTemperature(temperature);

        _taskLogger.logPeriodAndExecutionTime(
            _timer, advembsof::TaskLogger::kDisplayTaskIndex, taskStartTime);

        ThisThread::sleep_for(5000ms);

        // register the time at the end of the cyclic schedule period and print the
        // elapsed time for the period
        std::chrono::microseconds endTime = _timer.elapsed_time();
        const auto cycle =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        tr_debug("Repeating cycle time is %" PRIu64 " milliseconds", cycle.count());
    }
}

}  // namespace static_scheduling
