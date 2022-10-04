#pragma once

#include "mbed.h"

namespace static_scheduling_with_event {

class TaskLogger {
public:
    void logPeriodAndExecutionTime(Timer &timer, int taskIndex, const std::chrono::microseconds &taskStartTime);

    static constexpr int kCountTaskIndex = 0;
    static constexpr int kDisplayTaskIndex = 1;
    
private:
    static constexpr int kNbrOfTasks = 2;
    static constexpr char *kTaskDescriptors[] = { (char *) "Count", (char *) "Display"};
    std::chrono::microseconds _taskStartTime[kNbrOfTasks];
};

} // namespace static_scheduling_with_event