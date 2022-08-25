#pragma once

#include "mbed.h"

namespace static_scheduling {

class LCDDisplay {
public:

    LCDDisplay();

    // method called for displaying information
    void show(uint8_t currentGear, uint32_t currentRotationCount, uint8_t subTaskIndex = UINT8_MAX);

private:
    // data members
    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 400ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 300000us;
    static constexpr std::chrono::microseconds kSubTaskRunTime = 100000us;
    // nbr of subtasks for static cyclic scheduling
    static constexpr int kNbrOfSubTasks = 3;
};

} // namespace static_scheduling
