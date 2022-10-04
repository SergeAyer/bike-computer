#include "multi_tasking/bike_system.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

BikeSystem::BikeSystem() : 
  _resetDevice(callback(this, &BikeSystem::setReset)),
  _gearSystemDevice(callback(this, &BikeSystem::setNewGear)),
  _wheelCounterDevice(_countQueue),
  _lcdDisplay(_processedMail)
{
}

void BikeSystem::start()
{
    tr_info("Starting multi-tasking bike system");

    // start the timer
    _timer.start();

    // the wheel counter and lcd display must be started
    // the reset and gear system device are event-based

    // start the wheel counter device
    _wheelCounterDevice.start();

    // start the lcd display
    _lcdDisplay.start();

    tr_debug("Bike system started");

    // dispatch event on the ISR queue forever (will not return)
    _eventQueueForISRs.dispatch_forever();
}

void BikeSystem::setNewGear() 
{
    // defer the job to the event queue
    _eventQueueForISRs.call(mbed::callback(this, &BikeSystem::updateCurrentGear));
}

void BikeSystem::updateCurrentGear()
{
    // get the new gear 
    uint32_t gear = _gearSystemDevice.getCurrentGear();
    core_util_atomic_store_u32(&_currentGear, gear);
}


void BikeSystem::performReset()
{
    tr_info("Reset task: response time is %d usecs", (int)(_timer.elapsed_time().count() - _resetTime.count()));

    core_util_atomic_store_u32(&_totalRotationCount, 0);  
}


void BikeSystem::setReset() 
{
    _resetTime = _timer.elapsed_time();
    
    // defer the job to the event queue
    _eventQueueForISRs.call(mbed::callback(this, &BikeSystem::performReset));
}

} // namespace multi_tasking