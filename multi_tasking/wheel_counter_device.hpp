#pragma once

#include "multi_tasking/data_types.hpp"

#include "mbed.h"

namespace multi_tasking {

class WheelCounterDevice {
public:
    WheelCounterDevice(CountQueue &countQueue);

    // the wheel counter device runs its own thread
    void start();

private:
    // definition of wheel rotation time
    static constexpr std::chrono::milliseconds kWheelRotationTime = 200ms;

    // data members
    LowPowerTicker _ticker;    
    uint32_t _rotationCount = 0;
    CountQueue &_countQueue;

    // private methods
    void turn();
};

} // namespace multi_tasking