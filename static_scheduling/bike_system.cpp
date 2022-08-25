#include "static_scheduling/bike_system.hpp"
#include "static_scheduling/task_logger.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

// for hidding the logging from the header file, declare a global variable for logging
static TaskLogger gTaskLogger;

BikeSystem::BikeSystem() : _resetDevice(_timer)
{
}

void BikeSystem::start()
{
    tr_info("Starting Super-Loop with no event");

    // start the timer
    _timer.start();

    while (true) {
        // register the time at the beginning of the cyclic schedule period
        std::chrono::microseconds startTime = _timer.elapsed_time();

        // perform tasks as documented in the timetable
        updateCurrentGear();
        updateWheelRotationCount();
        updateDisplay(0);
        checkAndPerformReset();
        updateWheelRotationCount();
        updateDisplay(1);
        updateCurrentGear();
        updateWheelRotationCount();
        updateDisplay(2);
        checkAndPerformReset();
        updateWheelRotationCount();

        ThisThread::sleep_for(std::chrono::milliseconds(100));

        // register the time at the end of the cyclic schedule period and print the elapsed time for the period
        std::chrono::microseconds endTime = _timer.elapsed_time();
        tr_debug("Repeating cycle time is %d milliseconds", (int)(endTime.count() - startTime.count()) / 1000);
    }
}

void BikeSystem::updateCurrentGear()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _gear = _gearSystemDevice.getCurrentGear();

    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kGearTaskIndex, taskStartTime);
}

void BikeSystem::updateWheelRotationCount()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _wheelRotationCount = _wheelCounterDevice.getCurrentRotationCount();

    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kCountTaskIndex, taskStartTime);
}

void BikeSystem::updateDisplay(int subTaskIndex)
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _lcdDisplay.show(_gear, _wheelRotationCount, subTaskIndex);

    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kDisplayTaskIndex, taskStartTime);
}

void BikeSystem::checkAndPerformReset()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    if (_resetDevice.checkReset()) {
        tr_info("Reset task: response time is %d usecs", (int)(_timer.elapsed_time().count() - _resetDevice.getFallTime().count()));
        _wheelCounterDevice.reset();
    }

    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kResetTaskIndex, taskStartTime);
}

} // namespace static_scheduling