#include "multi_threading/task_logger.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "TaskLogger"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_threading {

void TaskLogger::logPeriodAndExecutionTime(Timer &timer, int taskIndex, const std::chrono::microseconds &taskStartTime)
{
    std::chrono::microseconds periodTime = taskStartTime - _taskStartTime[taskIndex];
    _taskStartTime[taskIndex] = taskStartTime;
    std::chrono::microseconds taskEndTime = timer.elapsed_time();
    std::chrono::microseconds executionTime = taskEndTime - _taskStartTime[taskIndex];
    tr_debug("%s task: period %d usecs execution time %d usecs start time %d usecs",
             kTaskDescriptors[taskIndex],
             (int) periodTime.count(),
             (int) executionTime.count(),
             (int) _taskStartTime[taskIndex].count());
}

} // namespace multi_threading