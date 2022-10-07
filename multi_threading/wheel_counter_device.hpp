#pragma once

#include "mbed.h"

namespace multi_threading {

class WheelCounterDevice {
public:
    WheelCounterDevice(Timer &timer);

    // the wheel counter device runs its own thread
    void start();

private:
    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 400ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 200000us;    
    // definition of wheel rotation time
    static constexpr std::chrono::milliseconds kWheelRotationTime = 200ms;
    
    // data members
    Timer &_timer;
    Ticker _ticker;
    Thread _thread;
    EventQueue _eventQueue;

    // private methods
    void turn();
    void updateWheelRotationCount();
};

} // namespace multi_threading