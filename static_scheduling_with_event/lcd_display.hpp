#pragma once

#include "mbed.h"

namespace static_scheduling_with_event {

class LCDDisplay {
public:

    LCDDisplay();

    // method called for displaying information
    void show(uint8_t currentGear, uint32_t currentRotationCount, uint8_t subTaskIndex = UINT8_MAX);

private:
    // data members
    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 1600ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 300000us;
    // nbr of subtasks for static cyclic scheduling
    static constexpr uint8_t kNbrOfSubTasks = 2;
    // definition of subtask execution times
    static const std::chrono::microseconds kSubTaskRunTime[kNbrOfSubTasks];
    
};

} // namespace static_scheduling_with_event
