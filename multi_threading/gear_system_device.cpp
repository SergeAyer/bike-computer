#include "multi_threading/gear_system_device.hpp"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "GearSystemDevice"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_threading {

GearSystemDevice::GearSystemDevice(mbed::Callback<void()> cb) : _usbSerial(true)
{
    // attach the callback to the serial port
    _usbSerial.attach(cb);
}

int GearSystemDevice::getCurrentGear()
{
    // when a callback is registered, we need to read data
    // on the serial port upon getting the current gear
    while (_usbSerial.available()) {
      // printf("Got character %c\n", m_usbSerial.getc());
      char str[2] = { (char) _usbSerial.getc(), 0};
      _currentGear = atoi(str);
    }

    return _currentGear;
}

} // namespace multi_tasking