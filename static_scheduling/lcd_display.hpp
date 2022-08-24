#pragma once

#include "mbed.h"

namespace static_scheduling {

class LCDDisplay {
public:

    LCDDisplay();

    // method called for displaying information
    void show(int currentGear, int currentRotationCount, int subTaskIndex = -1);

    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 1600ms;

private:
    // data members
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 300000us;
    static constexpr std::chrono::microseconds kSubTaskRunTime = 100000us;

    // for simulating a task run time, we sleep for this time in the show() method

    static constexpr int kNbrOfSubTasks = 3;
};

} // namespace static_scheduling
