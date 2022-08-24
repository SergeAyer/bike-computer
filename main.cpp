#include "mbed.h"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "main"
#endif // MBED_CONF_MBED_TRACE_ENABLE

#include "static_scheduling/bike_system.hpp"

// main() runs in its own thread in the OS
int main()
{
    // use trace library for console output
    mbed_trace_init();
 
    // declare the BikeSystem instance
    static_scheduling::BikeSystem bikeSystem;

    // will start the BikeSystem and never return
    tr_debug("Starting the bike system");
    bikeSystem.start();
}

