#pragma once

#include "mbed.h"

namespace multi_tasking {

#if defined(MBED_ALL_STATS_ENABLED)

class MemoryLogger {
public:
    // methods used by owners
    void getAndPrintStatistics();
    void printDiffs();
    void printRuntimeMemoryMap();
    
    void getAndPrintHeapStatistics();
    void getAndPrintStackStatistics();
    
private:
    // data members
    static constexpr uint8_t kMaxThreadInfo = 10;
    mbed_stats_heap_t _heapInfo = {0};
    mbed_stats_stack_t _stackInfo[kMaxThreadInfo] = {0};
    mbed_stats_stack_t _globalStackInfo = {0};
};

#endif // MBED_ALL_STATS_ENABLED

} // namespace multi_tasking
