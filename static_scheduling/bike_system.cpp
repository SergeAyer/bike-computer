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

BikeSystem::BikeSystem() : _resetDevice(_timer), _speedometer(_timer) {}

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
        std::chrono::microseconds startTime = _timer.elapsed_time();

        // update gear
        const auto gear = readCurrentGear();
        updateGearOnDisplay(gear);

        // update speed and distance
        const auto pedalRotationTime = readCurrentPedalRotationTime();
        const auto gearSize          = readCurrentGearSize();
        _speedometer.setCurrentRotationTime(pedalRotationTime);
        _speedometer.setGearSize(gearSize);
        float speed = readSpeed();
        updateSpeedOnDisplay(speed);
        float distance = readDistance();
        updateDistanceOnDisplay(distance);

        // update temperature
        float temperature = readTemperature();
        updateTemperatureOnDisplay(temperature);

        // check for reset
        checkAndPerformReset();

        ThisThread::sleep_for(5000ms);

        // register the time at the end of the cyclic schedule period and print the
        // elapsed time for the period
        std::chrono::microseconds endTime = _timer.elapsed_time();
        const auto cycle =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        tr_debug("Repeating cycle time is %" PRIu64 " milliseconds", cycle.count());
    }
}

uint8_t BikeSystem::readCurrentGear() {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    uint8_t gear = _gearDevice.getCurrentGear();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kGearTaskIndex, taskStartTime);

    return gear;
}

uint8_t BikeSystem::readCurrentGearSize() {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    uint8_t gearSize = _gearDevice.getCurrentGearSize();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kGearTaskIndex, taskStartTime);

    return gearSize;
}

std::chrono::milliseconds BikeSystem::readCurrentPedalRotationTime() {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    std::chrono::milliseconds pedalRotationTime = _pedalDevice.getCurrentRotationTime();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kGearTaskIndex, taskStartTime);

    return pedalRotationTime;
}

float BikeSystem::readSpeed() {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    float speed = _speedometer.getCurrentSpeed();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kCountTaskIndex, taskStartTime);

    return speed;
}

float BikeSystem::readDistance() {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    float distance = _speedometer.getDistance();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kCountTaskIndex, taskStartTime);

    return distance;
}

float BikeSystem::readTemperature() {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    float temperature = _sensorDevice.readTemperature();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kSensorTaskIndex, taskStartTime);

    return temperature;
}

void BikeSystem::updateGearOnDisplay(uint8_t gear) {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _displayDevice.displayGear(gear);

    std::chrono::microseconds taskEndTime   = _timer.elapsed_time();
    std::chrono::microseconds executionTime = taskEndTime - taskStartTime;
    tr_debug("Display gear %" PRIu64 " usecs", executionTime.count());

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kDisplayTaskIndex, taskStartTime);
}

void BikeSystem::updateSpeedOnDisplay(float speed) {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _displayDevice.displaySpeed(speed);

    std::chrono::microseconds taskEndTime   = _timer.elapsed_time();
    std::chrono::microseconds executionTime = taskEndTime - taskStartTime;
    tr_debug("Display speed %" PRIu64 " usecs", executionTime.count());

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kDisplayTaskIndex, taskStartTime);
}

void BikeSystem::updateDistanceOnDisplay(float distance) {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _displayDevice.displayDistance(distance);

    std::chrono::microseconds taskEndTime   = _timer.elapsed_time();
    std::chrono::microseconds executionTime = taskEndTime - taskStartTime;
    tr_debug("Display distance %" PRIu64 " usecs", executionTime.count());

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kDisplayTaskIndex, taskStartTime);
}

void BikeSystem::updateTemperatureOnDisplay(float temperature) {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _displayDevice.displayTemperature(temperature);

    std::chrono::microseconds taskEndTime   = _timer.elapsed_time();
    std::chrono::microseconds executionTime = taskEndTime - taskStartTime;
    tr_debug("Display temperature %" PRIu64 " usecs", executionTime.count());

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kDisplayTaskIndex, taskStartTime);
}

void BikeSystem::checkAndPerformReset() {
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    if (_resetDevice.checkReset()) {
        tr_info("Reset task: response time is %" PRIu64 " usecs",
                (_timer.elapsed_time() - _resetDevice.getFallTime()).count());
        _speedometer.reset();
    }

    _taskLogger.logPeriodAndExecutionTime(
        _timer, utils::TaskLogger::kResetTaskIndex, taskStartTime);
}

}  // namespace static_scheduling
