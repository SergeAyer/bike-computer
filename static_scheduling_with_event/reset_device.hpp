#pragma once

#include "mbed.h"

namespace static_scheduling_with_event {

class ResetDevice {
public:
    // constructor used for event-driven behavior
    ResetDevice(mbed::Callback<void()> cb);

private:

    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

    // data members
    // instance representing the reset button
    InterruptIn _resetButton;
};

} // namespace static_scheduling_with_event
