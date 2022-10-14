#pragma once

#include "multi_tasking/data_types.hpp"

#include "mbed.h"

namespace multi_tasking {

class WheelCounterDevice {
public:
    WheelCounterDevice(CountQueue &countQueue);

    // the wheel counter device runs its own thread
    void start();

    // constant defining the Wheel circumference (in meters)
    static constexpr uint32_t kWheelCircumference = 2;

private:
    // definition of wheel rotation time
    static constexpr std::chrono::milliseconds kWheelRotationTime = 200ms;
    static constexpr uint32_t kNbrOfRotationsForPut = 5;
    
    // data members
    LowPowerTicker _ticker;    
    uint32_t _rotationCount = 0;
    CountQueue &_countQueue;

    // private methods
    void turn();
};

} // namespace multi_tasking