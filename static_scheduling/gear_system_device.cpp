#include "static_scheduling/gear_system_device.hpp"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "GearSystemDevice"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

GearSystemDevice::GearSystemDevice() : _usbSerial(true)
{
    _thread.start(callback(this, &GearSystemDevice::read));
}

void GearSystemDevice::read()
{
    while (true) {
        if (_usbSerial.available()) {
            char str[2] = { (char) _usbSerial.getc(), 0};
            _currentGear = atoi(str);
            tr_info("New gear set to %d", _currentGear);
        }
        ThisThread::sleep_for(kReadingRate);
    }
}

int GearSystemDevice::getCurrentGear()
{
    // simulate task computation by waiting for the required task run time
    wait_us(kTaskRunTime.count());
    return _currentGear;
}

} // namespace static_scheduling