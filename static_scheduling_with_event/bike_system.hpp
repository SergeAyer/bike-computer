#pragma once

#include "gear_system_device.hpp"
#include "wheel_counter_device.hpp"
#include "reset_device.hpp"
#include "lcd_display.hpp"
#include <chrono>

namespace static_scheduling_with_event {

class BikeSystem {
public:
    // constructor
    BikeSystem();

    // method called in main() for starting the system
    void start();

private:
    // private methods 
    // called from the start() method super-loop for executing each periodic task
    void updateWheelRotationCount();
    void updateDisplay(uint8_t subTaskIndex);
    
    // for handling a reset event
    void setReset();
    void performReset();
    volatile bool _reset = false;
    std::chrono::microseconds _resetTime;

    // for handling a gear event
    void setNewGear();
    void updateCurrentGear();
    volatile bool _newGear = false;
 
    // definition of wait time
    static constexpr std::chrono::microseconds kTime_100ms = 100000us;
    
    // number of wheel rotation (read from the device)
    uint32_t _wheelRotationCount = 0;
    // current gear (read from the device)
    uint8_t _gear = 0;
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
};

} // namespace static_scheduling_with_event
