#include "multi_threading/thread_logger.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "ThreadLogger"
#endif // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_threading {

ThreadLogger::ThreadLogger(EventQueue &eventQueue)  : _eventQueue(eventQueue)
{

}

void ThreadLogger::start() 
{
    // start a ticker for signaling a wheel rotation
    _ticker.attach(callback(this, &ThreadLogger::log), kLogThreadPeriod);
}

void ThreadLogger::log() 
{   
    // ISR context -> defer to eventQueue
    _eventQueue.call(callback(this, &ThreadLogger::getThreadStats));
}

void ThreadLogger::getThreadStats()
{
    static constexpr uint8_t kMaxThreads = 8;
    mbed_stats_thread_t  stats[kMaxThreads] = {0};
    
    int count = mbed_stats_thread_get_each(stats, kMaxThreads);
    for (int i = 0; i < count; i++) {
        if (stats[i].state == osThreadRunning) {
            printf("ID: 0x%" PRIx32 "\n", stats[i].id);
            printf("Name: %s \n", stats[i].name);
            printf("State: %" PRId32 "\n", stats[i].state);
            printf("Priority: %" PRId32 "\n", stats[i].priority);
            printf("Stack Size: %" PRId32 "\n", stats[i].stack_size);
            printf("Stack Space: %" PRId32 "\n", stats[i].stack_space);
            printf("\n");
        }
    }
}

}  // namespace multi_threading
