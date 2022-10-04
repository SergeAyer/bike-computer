#pragma once

#include "mbed.h"

namespace multi_threading {

class LCDDisplay {
public:
    LCDDisplay(Timer &timer);

    // the LCD display runs its own thread
    void start();

private:
    // private methods
    void displayInfo();

    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 1600ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 300000us;
   
    // data members
    Timer &_timer;
    Thread _thread;
    EventQueue _eventQueue;
};

} // namespace multi_threading
