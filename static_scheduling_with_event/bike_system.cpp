#include "static_scheduling_with_event/bike_system.hpp"
#include "static_scheduling_with_event/task_logger.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling_with_event {

// for hidding the logging from the header file, declare a global variable for logging
static TaskLogger gTaskLogger;

BikeSystem::BikeSystem() : 
    _resetDevice(callback(this, &BikeSystem::setReset)),
    _gearSystemDevice(callback(this, &BikeSystem::setNewGear))
{
}

void BikeSystem::start()
{
    tr_info("Starting Super-Loop with event handling");

    // start the timer
    _timer.start();

    while (true) {
        // register the time at the beginning of the cyclic schedule period
        std::chrono::microseconds startTime = _timer.elapsed_time();

        // perform tasks as documented in the timetable
        updateWheelRotationCount();
        updateDisplay(0);
        updateWheelRotationCount();
        updateDisplay(1);

        if (_reset) {
          performReset();
          _reset = false;
        }
        wait_us(kTime_100ms.count());

        updateWheelRotationCount();

        if (_newGear) {
          updateCurrentGear();
          _newGear = false;
        }
        wait_us(kTime_100ms.count());

        if (_reset) {
          performReset();
          _reset = false;
        }        
        wait_us(kTime_100ms.count());

        updateWheelRotationCount();

        if (_newGear) {
          updateCurrentGear();
          _newGear = false;
        }
        wait_us(kTime_100ms.count());

        if (_reset) {
          performReset();
          _reset = false;
        }        
        wait_us(kTime_100ms.count());

        // register the time at the end of the cyclic schedule period and print the elapsed time for the period
        std::chrono::microseconds endTime = _timer.elapsed_time();
        tr_debug("Repeating cycle time is %d milliseconds", (int)(endTime.count() - startTime.count()) / 1000);
    }
}

void BikeSystem::setNewGear() 
{
    _newGear = true;
}

void BikeSystem::updateCurrentGear()
{
    // get the new gear 
    uint32_t gear = _gearSystemDevice.getCurrentGear();
    core_util_atomic_store_u8(&_gear, gear);
}

void BikeSystem::updateWheelRotationCount()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _wheelRotationCount = _wheelCounterDevice.getCurrentRotationCount();

    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kCountTaskIndex, taskStartTime);
}

void BikeSystem::updateDisplay(uint8_t subTaskIndex)
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _lcdDisplay.show(_gear, _wheelRotationCount, subTaskIndex);

    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kDisplayTaskIndex, taskStartTime);
}

void BikeSystem::performReset()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    tr_info("Reset task: response time is %d usecs", (int)(_timer.elapsed_time().count() - _resetTime.count()));
    _wheelCounterDevice.reset();
}

void BikeSystem::setReset() 
{
    _resetTime = _timer.elapsed_time();
    _reset = true;
}

} // namespace static_scheduling_with_event