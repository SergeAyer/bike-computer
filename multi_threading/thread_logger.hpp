#pragma once

#include "mbed.h"

namespace multi_threading {

class ThreadLogger {
public:
    ThreadLogger(EventQueue &eventQueue);

    // method called for starting the thread logger
    void start();

private:
    // private methods
    void log();
    void getThreadStats();
    
    // data members    
    static constexpr std::chrono::milliseconds kLogThreadPeriod = 1000ms;
    Ticker _ticker;
    EventQueue &_eventQueue;
};

} // namespace multi_threading