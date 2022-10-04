#include "multi_tasking/reset_device.hpp"

#if defined(TARGET_DISCO_L475VG_IOT01A)
#define PUSH_BUTTON BUTTON1
#define POLARITY_PRESSED 0
#endif

namespace multi_tasking {

ResetDevice::ResetDevice(mbed::Callback<void()> cb) : _resetButton(PUSH_BUTTON)
{
    // register a callback for computing the response time
    _resetButton.fall(cb);
}

} // namespace multi_tasking

