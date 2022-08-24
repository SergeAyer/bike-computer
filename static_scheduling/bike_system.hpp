#pragma once

#include "gear_system_device.hpp"
#include "wheel_counter_device.hpp"
#include "reset_device.hpp"
#include "lcd_display.hpp"

namespace static_scheduling {

class BikeSystem {
public:
    BikeSystem();

    void start();

private:
    void updateCurrentGear();
    void updateWheelRotationCount();

    void checkAndPerformReset();
    void updateDisplay(int subTaskIndex);

    // number of wheel rotation (read from the device)
    int _wheelRotationCount = 0;
    // current gear (read from the device)
    int _gear = 0;
    // data member that represents the device for manipulating the gear
    GearSystemDevice _gearSystemDevice;
    // data member that represents the device for counting wheel rotations
    WheelCounterDevice _wheelCounterDevice;
    // data member that represents the device used for resetting
    ResetDevice _resetDevice;
    // data member that represents the device display
    LCDDisplay _lcdDisplay;

    void logPeriodAndExecutionTime(int taskIndex, const std::chrono::microseconds &taskStartTime);
    Timer _timer;
    static constexpr int kNbrOfTasks = 4;
    static constexpr int kResetTaskIndex = 0;
    static constexpr int kGearTaskIndex = 1;
    static constexpr int kCountTaskIndex = 2;
    static constexpr int kDisplayTaskIndex = 3;
    static constexpr char *kTaskDescriptors[] = { (char *) "Reset", (char *) "Gear", (char *) "Count", (char *) "Display"};
    std::chrono::microseconds _taskStartTime[kNbrOfTasks];
};

} // namespace static_scheduling
