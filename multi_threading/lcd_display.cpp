#include "multi_threading/lcd_display.hpp"
#include "multi_threading/task_logger.hpp"
#include "multi_threading/wait.h"

namespace multi_threading {

// for hidding the logging from the header file, declare a global variable for logging
extern TaskLogger gTaskLogger;

LCDDisplay::LCDDisplay(Timer &timer) : 
    _timer(timer),
    _thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "LCDDisplay")
{
}

void LCDDisplay::start() 
{
    // schedule display updates
    _eventQueue.call_every(LCDDisplay::kTaskPeriod, callback(this, &LCDDisplay::displayInfo));
    _thread.start(callback(&_eventQueue, &EventQueue::dispatch_forever));
}

void LCDDisplay::displayInfo()
{
    std::chrono::microseconds taskStartTime = _timer.elapsed_time();

    //wait_us(kTaskRunTime.count());
    busy_wait_ms(std::chrono::duration_cast<std::chrono::milliseconds>(kTaskRunTime).count());

    // printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
    
    gTaskLogger.logPeriodAndExecutionTime(_timer, TaskLogger::kDisplayTaskIndex, taskStartTime);
}

} // namespace multi_threading
