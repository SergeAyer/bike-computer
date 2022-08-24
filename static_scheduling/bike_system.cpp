#include "static_scheduling/bike_system.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

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

    logPeriodAndExecutionTime(kGearTaskIndex, taskStartTime);
}

void BikeSystem::updateWheelRotationCount()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _wheelRotationCount = _wheelCounterDevice.getCurrentRotationCount();

    logPeriodAndExecutionTime(kCountTaskIndex, taskStartTime);
}

void BikeSystem::updateDisplay(int subTaskIndex)
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    _lcdDisplay.show(_gear, _wheelRotationCount, subTaskIndex);

    logPeriodAndExecutionTime(kDisplayTaskIndex, taskStartTime);
}

void BikeSystem::checkAndPerformReset()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    if (_resetDevice.checkReset()) {
        tr_info("Reset task: response time is %d usecs", (int)(_timer.elapsed_time().count() - _resetDevice.getFallTime().count()));
        _wheelCounterDevice.reset();
    }

    logPeriodAndExecutionTime(kResetTaskIndex, taskStartTime);
}

void BikeSystem::logPeriodAndExecutionTime(int taskIndex, const std::chrono::microseconds &taskStartTime)
{
    std::chrono::microseconds periodTime = taskStartTime - _taskStartTime[taskIndex];
    _taskStartTime[taskIndex] = taskStartTime;
    std::chrono::microseconds taskEndTime = _timer.elapsed_time();
    std::chrono::microseconds executionTime = taskEndTime - _taskStartTime[taskIndex];
    tr_debug("%s task: period %d usecs execution time %d usecs start time %d usecs",
             kTaskDescriptors[taskIndex],
             (int) periodTime.count(),
             (int) executionTime.count(),
             (int) _taskStartTime[taskIndex].count());
}

} // namespace static_scheduling