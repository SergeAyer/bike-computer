#include "multi_tasking/bike_system.hpp"

#include "mbed_trace.h"
#include <chrono>
#include <ratio>
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

BikeSystem::BikeSystem() :
    _processingThread(osPriorityNormal, OS_STACK_SIZE, nullptr, "ProcessingThread"),
    _resetDevice(callback(this, &BikeSystem::setReset)),
    _gearSystemDevice(callback(this, &BikeSystem::setNewGear)),
    _wheelCounterDevice(_countQueue),
    _lcdDisplay(_processedMail)
{
}

void BikeSystem::start()
{
    tr_info("Starting multi-tasking bike system");

    // start the timer
    _timer.start();

    // the wheel counter and lcd display must be started
    // the reset and gear system device are event-based

    // start the wheel counter device
    _wheelCounterDevice.start();

    // start the lcd display
    _lcdDisplay.start();

    tr_debug("Bike system started");

    // start the processing thread
    _processingThread.start(callback(this, &BikeSystem::processData));

    // dispatch event on the ISR queue forever (will not return)
    _eventQueueForISRs.dispatch_forever();
}

void BikeSystem::setNewGear()
{
    // defer the job to the event queue
    _eventQueueForISRs.call(mbed::callback(this, &BikeSystem::updateCurrentGear));
}

void BikeSystem::updateCurrentGear()
{
    // get the new gear
    uint32_t gear = _gearSystemDevice.getCurrentGear();
    core_util_atomic_store_u32(&_currentGear, gear);
}


void BikeSystem::performReset()
{
    tr_info("Reset task: response time is %d usecs", (int)(_timer.elapsed_time().count() - _resetTime.count()));

    core_util_atomic_store_u32(&_totalRotationCount, 0);
}


void BikeSystem::setReset()
{
    _resetTime = _timer.elapsed_time();

    // defer the job to the event queue
    _eventQueueForISRs.call(mbed::callback(this, &BikeSystem::performReset));
}

void BikeSystem::processData()
{
    // get the start time before entering the loop
    std::chrono::microseconds startTime;

    // process data forever
    while (true) {
        // get the rotation count
        uint32_t rotationCount = 0;
        _countQueue.try_get_for(Kernel::wait_for_u32_forever, (uint32_t **) &rotationCount);

        // at first rotation (upon reset), get the start time and wait for next rotation
        if (_totalRotationCount == 0) {
            startTime = _timer.elapsed_time();

            // update the rotation count
            _totalRotationCount += rotationCount;
        } 
        else {
            // get the elapsed time since last rotation count reset
            std::chrono::microseconds currentTime = _timer.elapsed_time();
            std::chrono::microseconds elapsedTimeSinceLastReset = currentTime - startTime;

            // update the rotation count
            _totalRotationCount += rotationCount;

            // compute distance in m
            long distance = WheelCounterDevice::kWheelCircumference * _totalRotationCount;
            
            // compute time in seconds
            std::chrono::duration<double> elapsedTimeInSecs = elapsedTimeSinceLastReset;
            
            // compute average rotation rate in rotation/sec
            float averageRotationRate = _totalRotationCount / elapsedTimeInSecs.count();

            // compute average speed in km/h
            // m/s -> km/h (x 3600/1000)
            float averageSpeed = (distance * 3600.0) / (elapsedTimeInSecs.count() * 1000.0);
            
            // push the new processed data
            ProcessedData *pProcessedData = _processedMail.try_alloc_for(Kernel::wait_for_u32_forever);
            pProcessedData->averageSpeed = averageSpeed;
            pProcessedData->averageRotationRate = averageRotationRate;
            _processedMail.put(pProcessedData);
        }
    }
}
} // namespace multi_tasking