#pragma once

#include "mbed.h"

namespace static_scheduling {

class ResetDevice {
public:
    // constructor
    ResetDevice(Timer &timer);

    // method called for checking the reset status
    bool checkReset();

    // for computing the response time
    const std::chrono::microseconds &getFallTime();

private:
    // called when the button is pressed
    void onFall();

    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

    // data members
    // instance representing the reset button
    InterruptIn _resetButton;
    Timer &_timer;
    std::chrono::microseconds _fallTime;
};

} // namespace static_scheduling
