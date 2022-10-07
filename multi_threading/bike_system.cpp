#include "multi_threading/bike_system.hpp"
#include "multi_threading/task_logger.hpp"
#include "multi_threading/thread_logger.hpp"
#include "multi_threading/busy_wait.h"

#include "mbed_trace.h"

#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeSystem"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_threading {

// for hidding the logging from the header file, declare a global variable for logging
TaskLogger gTaskLogger;

// global variables used for exchanging information among threads
// this is of course not a proper way of exchanging information in a 
// multi-threaded application
uint8_t gCurrentGear = 0;
uint32_t gCurrentRotationCount = 0;

BikeSystem::BikeSystem() : 
  _resetDevice(callback(this, &BikeSystem::setReset)),
  _gearSystemDevice(callback(this, &BikeSystem::setNewGear)),
  _lcdDisplay(_timer),
  _wheelCounterDevice(_timer)
{
}

void BikeSystem::start()
{
    tr_info("Starting multi-threading bike system");

    // start the timer
    _timer.start();

    // test the wait function
    for (uint32_t i = 1; i < 3; i++) {
      std::chrono::microseconds startTime = _timer.elapsed_time();
      busy_wait_ms(i * 1000);
      std::chrono::microseconds endTime = _timer.elapsed_time();
      tr_debug("Wait time is %lld usecs", endTime.count() - startTime.count());
    }
    
    // start the thread logger
    //ThreadLogger threadLogger(_eventQueueForISRs);
    //threadLogger.start();

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
    core_util_atomic_store_u8(&gCurrentGear, gear);
}


void BikeSystem::performReset()
{
    tr_info("Reset task: response time is %d usecs", (int)(_timer.elapsed_time().count() - _resetTime.count()));

    core_util_atomic_store_u32(&gCurrentRotationCount, 0);  
}


void BikeSystem::setReset() 
{
    _resetTime = _timer.elapsed_time();
    
    // defer the job to the event queue
    _eventQueueForISRs.call(mbed::callback(this, &BikeSystem::performReset));
}

} // namespace multi_threading