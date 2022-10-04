#pragma once

#include "gear_system_device.hpp"
#include "wheel_counter_device.hpp"
#include "reset_device.hpp"
#include "lcd_display.hpp"
#include <chrono>

namespace multi_threading {

class BikeSystem {
public:
    // constructor
    BikeSystem();

    // method called in main() for starting the system
    void start();

private:
    // private methods 
    void setNewGear();
    void updateCurrentGear();
    void performReset();
    
    // called as reset handler
    void setReset();
    std::chrono::microseconds _resetTime;
 
    // EventQueue used for serving deferred ISRs
    EventQueue _eventQueueForISRs;

    // timer instance used for loggint task time and used by ResetDevice
    Timer _timer;
    // data member that represents the device for manipulating the gear
    GearSystemDevice _gearSystemDevice;
    // data member that represents the device for counting wheel rotations
    WheelCounterDevice _wheelCounterDevice;
    // data member that represents the device used for resetting
    ResetDevice _resetDevice;
    // data member that represents the device display
    LCDDisplay _lcdDisplay;
    // total rotation count
    volatile uint32_t _totalRotationCount = 0;
    // current gear
    volatile uint32_t _currentGear = 0;
};

} // namespace multi_threading
