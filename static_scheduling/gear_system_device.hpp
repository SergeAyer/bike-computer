#pragma once

#include "mbed.h"

#include "USBSerial.h"

namespace static_scheduling {

class GearSystemDevice {
public:
    // constructor used for simulating the device with a thread
    GearSystemDevice();

    // method called for updating the bike system
    int getCurrentGear();

private:
    // reading rate in milliseconds when running a separate thread
    // The gear value is updated every second
    static constexpr std::chrono::milliseconds kReadingRate = 1000ms;

    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime = 100000us;

    // methods
    void read();

    // data members
    USBSerial _usbSerial;
    uint8_t _currentGear = 0;
    Thread _thread;
};

} // namespace static_scheduling