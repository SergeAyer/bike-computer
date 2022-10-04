#include "static_scheduling_with_event/wheel_counter_device.hpp"

namespace static_scheduling_with_event {

WheelCounterDevice::WheelCounterDevice()
{
    // start the ticker for incrementing wheel counter
    _ticker.attach(callback(this, &WheelCounterDevice::turn), kWheelRotationTime);
}

int WheelCounterDevice::getCurrentRotationCount()
{
    // simulate task computation by waiting for the required task run time
    wait_us(kTaskRunTime.count());
    return _rotationCount;
}

void WheelCounterDevice::reset()
{
    _rotationCount = 0;
}

void WheelCounterDevice::turn()
{
    // ISR context
    // increment rotation count
    // the call for incrementing the count should be atomic
    _rotationCount++;
}

} // namespace static_scheduling_with_event
