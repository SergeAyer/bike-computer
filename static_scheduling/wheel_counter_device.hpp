#pragma once

#include "mbed.h"

namespace static_scheduling {

class WheelCounterDevice {
public:
    WheelCounterDevice();

    // method called for getting the current wheel rotation count
    int getCurrentRotationCount();

    // method called for resetting the counter
    void reset();

    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 400ms;

private:
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 200000us;
    // definition of wheel rotation time
    static constexpr std::chrono::milliseconds kWheelRotationTime = 200ms;

    // data members
    LowPowerTicker _ticker;
    int _rotationCount = 0;

    // private methods
    void turn();
};

} // namespace static_scheduling