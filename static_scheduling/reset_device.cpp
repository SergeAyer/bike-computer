#include "reset_device.hpp"

#if defined(TARGET_DISCO_L475VG_IOT01A)
#define PUSH_BUTTON BUTTON1
#define POLARITY_PRESSED 0
#endif

namespace static_scheduling {

ResetDevice::ResetDevice(Timer &timer) :
    _timer(timer),
    _resetButton(PUSH_BUTTON)
{
    // register a callback for computing the response time
    _resetButton.fall(callback(this, &ResetDevice::onFall));
}

void ResetDevice::onFall()
{
    _fallTime = _timer.elapsed_time();
}

const std::chrono::microseconds &ResetDevice::getFallTime()
{
    return _fallTime;
}

bool ResetDevice::checkReset()
{
    // simulate task computation by waiting for the required task run time
    wait_us(kTaskRunTime.count());

    return _resetButton.read() == POLARITY_PRESSED;
}

} // namespace static_scheduling

