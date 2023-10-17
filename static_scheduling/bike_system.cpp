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

#include "bike_system.hpp"

#include <chrono>

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

static constexpr std::chrono::microseconds kTemperatureTaskRunTime = 100ms;
static constexpr std::chrono::microseconds kDisplayTask1RunTime    = 200ms;
static constexpr std::chrono::microseconds kDisplayTask2RunTime    = 100ms;

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
        auto startTime = _timer.elapsed_time();

        // schedule tasks
        uint8_t gear     = 0;
        uint8_t gearSize = 0;
        gearTask(gear, gearSize);
        float speed    = 0.0f;
        float distance = 0.0f;
        speedDistanceTask(gearSize, speed, distance);
        displayTask1(gear, speed, distance);
        speedDistanceTask(gearSize, speed, distance);
        resetTask();
        gearTask(gear, gearSize);
        speedDistanceTask(gearSize, speed, distance);
        float temperature = 0;
        temperatureTask(temperature);
        displayTask2(temperature);
        speedDistanceTask(gearSize, speed, distance);
        resetTask();

        // register the time at the end of the cyclic schedule period and print the
        // elapsed time for the period
        std::chrono::microseconds endTime = _timer.elapsed_time();
        const auto cycle =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        tr_debug("Repeating cycle time is %" PRIu64 " milliseconds", cycle.count());

        bool stopFlag = false;
        core_util_atomic_load(&stopFlag);
        if (stopFlag) {
            break;
        }
    }
}

void BikeSystem::stop() { core_util_atomic_store_bool(&_stopFlag, true); }

#if defined(MBED_TEST_MODE)
const advembsof::TaskLogger& BikeSystem::getTaskLogger() { return _taskLogger; }
#endif  // defined(MBED_TEST_MODE)

void BikeSystem::gearTask(uint8_t& gear, uint8_t& gearSize) {
    // gear task
    auto taskStartTime = _timer.elapsed_time();

    gear     = _gearDevice.getCurrentGear();
    gearSize = _gearDevice.getCurrentGearSize();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kGearTaskIndex, taskStartTime);
}

void BikeSystem::speedDistanceTask(uint8_t gearSize, float& speed, float& distance) {
    // speed and distance task
    auto taskStartTime = _timer.elapsed_time();

    const auto pedalRotationTime = _pedalDevice.getCurrentRotationTime();
    _speedometer.setCurrentRotationTime(pedalRotationTime);
    _speedometer.setGearSize(gearSize);
    speed    = _speedometer.getCurrentSpeed();
    distance = _speedometer.getDistance();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kSpeedTaskIndex, taskStartTime);
}

void BikeSystem::temperatureTask(float& temperature) {
    auto taskStartTime = _timer.elapsed_time();

    temperature = _sensorDevice.readTemperature();

    // simulate task computation by waiting for the required task run time

    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    while (elapsedTime < kTemperatureTaskRunTime) {
        elapsedTime = _timer.elapsed_time() - taskStartTime;
    }

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kTemperatureTaskIndex, taskStartTime);
}

void BikeSystem::resetTask() {
    auto taskStartTime = _timer.elapsed_time();

    if (_resetDevice.checkReset()) {
        tr_info("Reset task: response time is %" PRIu64 " usecs",
                (_timer.elapsed_time() - _resetDevice.getFallTime()).count());
        _speedometer.reset();
    }

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kResetTaskIndex, taskStartTime);
}

void BikeSystem::displayTask1(uint8_t gear, float speed, float distance) {
    auto taskStartTime = _timer.elapsed_time();

    _displayDevice.displayGear(gear);
    _displayDevice.displaySpeed(speed);
    _displayDevice.displayDistance(distance);

    // simulate task computation by waiting for the required task run time

    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    while (elapsedTime < kDisplayTask1RunTime) {
        elapsedTime = _timer.elapsed_time() - taskStartTime;
    }

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask1Index, taskStartTime);
}

void BikeSystem::displayTask2(float temperature) {
    auto taskStartTime = _timer.elapsed_time();

    _displayDevice.displayTemperature(temperature);

    // simulate task computation by waiting for the required task run time

    std::chrono::microseconds elapsedTime = std::chrono::microseconds::zero();
    while (elapsedTime < kDisplayTask2RunTime) {
        elapsedTime = _timer.elapsed_time() - taskStartTime;
    }
    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask2Index, taskStartTime);
}

}  // namespace static_scheduling
