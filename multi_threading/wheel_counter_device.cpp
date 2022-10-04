#include "multi_threading/wheel_counter_device.hpp"
#include "multi_threading/task_logger.hpp"
#include "multi_threading/wait.h"

namespace multi_threading {

// for hidding the logging from the header file, declare a global variable for logging
extern TaskLogger gTaskLogger;

WheelCounterDevice::WheelCounterDevice(Timer &timer) : 
    _timer(timer),
    _thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "WheelCounterDevice")
{    
}

void WheelCounterDevice::start() 
{
    // start a ticker for signaling a wheel rotation
    _ticker.attach(callback(this, &WheelCounterDevice::turn), kWheelRotationTime);

    // schedule wheel rotation updates
    _eventQueue.call_every(WheelCounterDevice::kTaskPeriod, callback(this, &WheelCounterDevice::updateWheelRotationCount));
    _thread.start(callback(&_eventQueue, &EventQueue::dispatch_forever));
}

void WheelCounterDevice::turn()
{
    // ISR context
    // increment rotation count
    _rotationCount++;
}

void WheelCounterDevice::updateWheelRotationCount()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    // simulate task computation by waiting for the required task run time
    //wait_us(kTaskRunTime.count());
    busy_wait_ms(std::chrono::duration_cast<std::chrono::milliseconds>(kTaskRunTime).count());

    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kCountTaskIndex, taskStartTime);
}

} // namespace multi_threading
