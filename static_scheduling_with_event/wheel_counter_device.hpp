#pragma once

#include "mbed.h"

namespace static_scheduling_with_event {

class WheelCounterDevice {
public:
    WheelCounterDevice();

    // method called for getting the current wheel rotation count
    int getCurrentRotationCount();

    // method called for resetting the counter
    void reset();

private:
    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 400ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 200000us;
    // definition of wheel rotation time
    static constexpr std::chrono::milliseconds kWheelRotationTime = 200ms;

    // data members
    LowPowerTicker _ticker;
    uint32_t _rotationCount = 0;

    // private methods
    void turn();
};

} // namespace static_scheduling_with_event