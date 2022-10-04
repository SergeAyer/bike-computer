#pragma once

#include "mbed.h"

namespace multi_threading {

class TaskLogger {
public:
    void logPeriodAndExecutionTime(Timer &timer, int taskIndex, const std::chrono::microseconds &taskStartTime);

    static constexpr int kResetTaskIndex = 0;
    static constexpr int kGearTaskIndex = 1;
    static constexpr int kCountTaskIndex = 2;
    static constexpr int kDisplayTaskIndex = 3;
    
private:
    static constexpr int kNbrOfTasks = 4;
    static constexpr char *kTaskDescriptors[] = { (char *) "Reset", (char *) "Gear", (char *) "Count", (char *) "Display"};
    std::chrono::microseconds _taskStartTime[kNbrOfTasks];
};

} // namespace multi_threading