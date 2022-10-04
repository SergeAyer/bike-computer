#pragma once

#include "mbed.h"

#include "USBSerial.h"

namespace multi_tasking {

class GearSystemDevice {
public:
    // constructor used for simulating the device with a thread
    GearSystemDevice(mbed::Callback<void()> cb);

    // method called for updating the bike system
    int getCurrentGear();

private:   
    // data members
    USBSerial _usbSerial;
    uint8_t _currentGear = 0;
};

} // namespace multi_tasking