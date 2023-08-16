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
 * @brief Bike System implementation
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#include "multi_tasking/bike_system.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

BikeSystem::BikeSystem()
    : _processingThread(osPriorityNormal, OS_STACK_SIZE, nullptr, "ProcessingThread"),
      _gearSystemDevice(callback(this, &BikeSystem::setNewGear)),
      _wheelCounterDevice(_countQueue),
      _resetDevice(callback(this, &BikeSystem::setReset)),
      _bikeDisplay(_processedMail) {}

void BikeSystem::start() {
    tr_info("Starting multi-tasking bike system (update 2)");

    // start the timer
    _timer.start();

    // the wheel counter and lcd display must be started
    // the reset and gear system device are event-based

    // start the wheel counter device
    _wheelCounterDevice.start();

    // start the processing thread
    _processingThread.start(callback(this, &BikeSystem::processData));

    // start the lcd display
    _bikeDisplay.start();

    tr_debug("Bike system started");

#if defined(USE_MEMORY_LOGGER)
    // log memory state
    _memoryLogger.getAndPrintStatistics();
    _memoryLogger.printRuntimeMemoryMap();
#endif

    // dispatch event on the ISR queue forever (will not return)
    _eventQueueForISRs.dispatch_forever();
}

void BikeSystem::setNewGear(uint8_t newGear) {
    // defer the job to the event queue
    _eventQueueForISRs.call(mbed::callback(this, &BikeSystem::updateCurrentGear),
                            newGear);
}

void BikeSystem::updateCurrentGear(uint8_t newGear) {
    // get the new gear
    //tr_debug("New gear is %d mmh", newGear);
    _bikeDisplay.displayGear(newGear);
}

void BikeSystem::performReset() {
    tr_info("Reset task: response time is %" PRIu64 " usecs",
            _timer.elapsed_time().count() - _resetTime.count());

    core_util_atomic_store_u32(&_totalRotationCount, 0);
}

void BikeSystem::setReset() {
    _resetTime = _timer.elapsed_time();

    // defer the job to the event queue
    _eventQueueForISRs.call(mbed::callback(this, &BikeSystem::performReset));
}

void BikeSystem::processData() {
    // get the start time before entering the loop
    std::chrono::microseconds startTime;

#if defined(MEMORY_FRAGMENTER)
    MemoryFragmenter memoryFragmenter;
    memoryFragmenter.fragmentMemory();
#endif

#if defined(STACK_OVERFLOW)
    MemoryStackOverflow memoryStackOverflow;
    static constexpr uint32_t kArraySize = 40;
    double doubleArray[kArraySize]       = {0};
    uint32_t multiplier                  = 1;
#endif

    // process data forever
    while (true) {
#if defined(MEMORY_LEAK)
        // create a memory leak
        MemoryLeak memLeak;
        memLeak.use();
        _memoryLogger.printDiffs();
#endif

#if defined(STACK_OVERFLOW)
        memoryStackOverflow.allocateOnStack();
        // allocate an array with growing size until it does not fit on the stack anymore
        uint32_t allocSize = kArraySize * multiplier;
        // Create a variable-size object on the stack
        double anotherArray[allocSize];  // NOLINT(runtime/arrays)
        for (size_t ix = 0; ix < allocSize; ix++) {
            anotherArray[ix] = ix;
        }
        // copy to member variable to prevent them from being optimized away
        for (int i = 0; i < kArraySize; i++) {
            doubleArray[i] += anotherArray[i];
        }
        multiplier++;
#endif

        // get the rotation count
        uint32_t rotationCount = 0;
        // NOLINTNEXTLINE(readability/casting)
        _countQueue.try_get_for(Kernel::wait_for_u32_forever, (uint32_t**)&rotationCount);

        // at first rotation (upon reset), get the start time and wait for next rotation
        if (_totalRotationCount == 0) {
            startTime = _timer.elapsed_time();

            // update the rotation count
            _totalRotationCount += rotationCount;
        } else {
            // get the elapsed time since last rotation count reset
            std::chrono::microseconds currentTime               = _timer.elapsed_time();
            std::chrono::microseconds elapsedTimeSinceLastReset = currentTime - startTime;

            // update the rotation count
            _totalRotationCount += rotationCount;

            // compute distance in m
            uint64_t distance =
                WheelCounterDevice::kWheelCircumference * _totalRotationCount;

            // compute time in seconds
            std::chrono::duration<double> elapsedTimeInSecs = elapsedTimeSinceLastReset;

            // compute average rotation rate in rotation/sec
            float averageRotationRate = _totalRotationCount / elapsedTimeInSecs.count();

            // compute average speed in km/h
            // m/s -> km/h (x 3600/1000)
            float averageSpeed =
                (distance * 3600.0) / (elapsedTimeInSecs.count() * 1000.0);

            // push the new processed data
            ProcessedData* pProcessedData =
                _processedMail.try_alloc_for(Kernel::wait_for_u32_forever);
            pProcessedData->averageSpeed        = averageSpeed;
            pProcessedData->averageRotationRate = averageRotationRate;
            _processedMail.put(pProcessedData);

            // log memory stats
#if defined(USE_MEMORY_LOGGER)
            _memoryLogger.printDiffs();
#endif
        }
    }
}
}  // namespace multi_tasking
