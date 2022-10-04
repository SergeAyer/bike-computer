#include "multi_tasking/wheel_counter_device.hpp"

namespace multi_tasking {

WheelCounterDevice::WheelCounterDevice(CountQueue &countQueue) : _countQueue(countQueue)
{    
}

void WheelCounterDevice::start() 
{
    // start a ticker for signaling a wheel rotation
    _ticker.attach(callback(this, &WheelCounterDevice::turn), kWheelRotationTime);
}

void WheelCounterDevice::turn()
{
    // ISR context
    // increment rotation count
    _rotationCount++;
    // try to push the data to the queue, if successful reset count
    if (_countQueue.try_put((uint32_t*) _rotationCount)) {
        _rotationCount = 0;
    } 
}

} // namespace multi_tasking
