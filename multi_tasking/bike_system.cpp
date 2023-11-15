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
 * @brief Bike System implementation (multi-tasking)
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

namespace multi_tasking {

static constexpr std::chrono::milliseconds kDisplayTaskPeriod              = 1600ms;
static constexpr std::chrono::milliseconds kDisplayTaskDelay               = 300ms;
static constexpr std::chrono::milliseconds kDisplayTaskComputationTime     = 200ms;
static constexpr std::chrono::milliseconds kTemperatureTaskPeriod          = 1600ms;
static constexpr std::chrono::milliseconds kTemperatureTaskDelay           = 1100ms;
static constexpr std::chrono::milliseconds kTemperatureTaskComputationTime = 100ms;
static constexpr std::chrono::milliseconds kMajorCycleDuration             = 1600ms;

BikeSystem::BikeSystem()
    : _gearDevice(_eventQueue, callback(this, &BikeSystem::onGearChanged)),
      _pedalDevice(_eventQueue, callback(this, &BikeSystem::onRotationSpeedChanged)),
      _resetDevice(callback(this, &BikeSystem::onReset)),
      _deferredISRThread(
          osPriorityAboveNormal, OS_STACK_SIZE, nullptr, "deferredISRThread"),
      _speedometer(_timer),
      _cpuLogger(_timer) {}

void BikeSystem::start() {
    tr_info("Starting Multi-tasking BikeComputer");

    init();

    Event<void()> displayEvent(&_eventQueue, callback(this, &BikeSystem::displayTask));
    displayEvent.delay(kDisplayTaskDelay);
    displayEvent.period(kDisplayTaskPeriod);
    displayEvent.post();

    Event<void()> temperatureEvent(&_eventQueue,
                                   callback(this, &BikeSystem::temperatureTask));
    temperatureEvent.delay(kTemperatureTaskDelay);
    temperatureEvent.period(kTemperatureTaskPeriod);
    temperatureEvent.post();

#if !defined(MBED_TEST_MODE)
    Event<void()> cpuStatsEvent(&_eventQueue,
                                callback(&_cpuLogger, &advembsof::CPULogger::printStats));
    cpuStatsEvent.delay(kMajorCycleDuration);
    cpuStatsEvent.period(kMajorCycleDuration);
    cpuStatsEvent.post();
#endif

    // start the thread for serving deferred ISRs
    _deferredISRThread.start(
        callback(&_eventQueueForISRs, &EventQueue::dispatch_forever));

    // print thread statistics
    _memoryLogger.getAndPrintThreadStatistics();

    _eventQueue.dispatch_forever();
}

void BikeSystem::stop() { core_util_atomic_store_bool(&_stopFlag, true); }

#if defined(MBED_TEST_MODE)
const advembsof::TaskLogger& BikeSystem::getTaskLogger() { return _taskLogger; }
#endif  // defined(MBED_TEST_MODE)

void BikeSystem::init() {
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
}

void BikeSystem::onReset() {
    _resetTime = _timer.elapsed_time();
    _eventQueueForISRs.call(callback(this, &BikeSystem::resetTask));
}

void BikeSystem::onGearChanged(uint8_t currentGear, uint8_t currentGearSize) {
    tr_debug("onGearChanged");
    _currentGear = currentGear;
    _speedometer.setGearSize(currentGearSize);
}

void BikeSystem::onRotationSpeedChanged(
    const std::chrono::milliseconds& pedalRotationTime) {
    _speedometer.setCurrentRotationTime(pedalRotationTime);
}

void BikeSystem::temperatureTask() {
    auto taskStartTime = _timer.elapsed_time();

    _currentTemperature = _sensorDevice.readTemperature();

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kTemperatureTaskIndex, taskStartTime);
}

void BikeSystem::resetTask() {
    tr_info("Reset task: response time is %" PRIu64 " usecs",
            (_timer.elapsed_time() - _resetTime).count());
    _speedometer.reset();
}

void BikeSystem::displayTask() {
    auto taskStartTime = _timer.elapsed_time();

    auto currentSpeed     = _speedometer.getCurrentSpeed();
    auto traveledDistance = _speedometer.getDistance();

    _displayDevice.displayGear(_currentGear);
    _displayDevice.displaySpeed(currentSpeed);
    _displayDevice.displayDistance(traveledDistance);
    _displayDevice.displayTemperature(_currentTemperature);

    _taskLogger.logPeriodAndExecutionTime(
        _timer, advembsof::TaskLogger::kDisplayTask1Index, taskStartTime);
}

}  // namespace multi_tasking
