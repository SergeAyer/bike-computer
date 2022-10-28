#include "memory_logger.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "MemoryLogger"
#endif // MBED_CONF_MBED_TRACE_ENABLE

#if defined(MBED_ALL_STATS_ENABLED)
extern unsigned char* mbed_stack_isr_start;
extern uint32_t mbed_stack_isr_size;
extern unsigned char* mbed_heap_start;
extern uint32_t mbed_heap_size;
#endif // MBED_ALL_STATS_ENABLED

namespace multi_tasking {

#if defined(MBED_ALL_STATS_ENABLED)

void MemoryLogger::printDiffs()
{
    {
        tr_debug("MemoryStats (Heap):");
        mbed_stats_heap_t heapInfo = {0};
        mbed_stats_heap_get(&heapInfo);
        int currentSizeDiff = heapInfo.current_size - _heapInfo.current_size;
        if (currentSizeDiff > 0) {
            tr_debug("\tBytes allocated increased by %d to %d bytes",
                     currentSizeDiff, heapInfo.current_size);
        }
        int maxSizeDiff = heapInfo.max_size - _heapInfo.max_size;
        if (maxSizeDiff > 0) {
            tr_debug("\tMax bytes allocated at a given time increased by %d to %d bytes (max heap size is %d bytes)",
                     maxSizeDiff, heapInfo.max_size, heapInfo.reserved_size);
        }
        _heapInfo = heapInfo;
    }
    {
        mbed_stats_stack_t globalStackInfo = {0};
        mbed_stats_stack_get(&globalStackInfo);
        tr_debug("Cumulative Stack Info:");
        int maxSizeDiff = globalStackInfo.max_size - _globalStackInfo.max_size;
        if (maxSizeDiff > 0) {
            tr_debug("\tMaximum number of bytes used on the stack increased by %d to %d bytes (stack size is %d bytes)",
                     maxSizeDiff, globalStackInfo.max_size, globalStackInfo.reserved_size);
        }
        uint32_t stackCntDiff = globalStackInfo.stack_cnt - _globalStackInfo.stack_cnt;
        if (stackCntDiff > 0) {
            tr_debug("\tNumber of stacks stats accumulated increased by %d to %d",
                     stackCntDiff, globalStackInfo.stack_cnt);
        }
        _globalStackInfo = globalStackInfo;

        mbed_stats_stack_t stackInfo[kMaxThreadInfo] = {0};
        mbed_stats_stack_get_each(stackInfo, kMaxThreadInfo);
        tr_debug("Thread Stack Info:");
        for (int i = 0; i < kMaxThreadInfo; i++) {
            if (stackInfo[i].thread_id != 0) {
                for (int j = 0; j < kMaxThreadInfo; j++) {
                    if (stackInfo[i].thread_id == _stackInfo[j].thread_id) {
                        int maxSizeDiff = stackInfo[i].max_size - _stackInfo[j].max_size;
                        if (maxSizeDiff > 0) {
                            tr_debug("\tThread: %d", j);
                            tr_debug("\t\tThread Id: 0x%08X with name %s",
                                     _stackInfo[j].thread_id,
                                     osThreadGetName((osThreadId_t) _stackInfo[j].thread_id));
                            tr_debug("\t\tMaximum number of bytes used on the stack increased by %d to %d bytes (stack size is %d bytes)",
                                     maxSizeDiff, stackInfo[i].max_size, stackInfo[i].reserved_size);
                        }
                        _stackInfo[j] = stackInfo[i];
                    }
                }
            }
        }
    }
}

void MemoryLogger::getAndPrintHeapStatistics()
{
    tr_debug("MemoryStats (Heap):");
    mbed_stats_heap_get(&_heapInfo);
    tr_debug("\tBytes allocated currently: %d", _heapInfo.current_size);
    tr_debug("\tMax bytes allocated at a given time: %d", _heapInfo.max_size);
    tr_debug("\tCumulative sum of bytes ever allocated: %d", _heapInfo.total_size);
    tr_debug("\tCurrent number of bytes allocated for the heap: %d", _heapInfo.reserved_size);
    tr_debug("\tCurrent number of allocations: %d", _heapInfo.alloc_cnt);
    tr_debug("\tNumber of failed allocations: %d", _heapInfo.alloc_fail_cnt);
}

void MemoryLogger::getAndPrintStackStatistics()
{
    mbed_stats_stack_get(&_globalStackInfo);
    tr_debug("Cumulative Stack Info:");
    tr_debug("\tMaximum number of bytes used on the stack: %d", _globalStackInfo.max_size);
    tr_debug("\tCurrent number of bytes allocated for the stack: %d", _globalStackInfo.reserved_size);
    tr_debug("\tNumber of stacks stats accumulated in the structure: %d", _globalStackInfo.stack_cnt);

    mbed_stats_stack_get_each(_stackInfo, kMaxThreadInfo);
    tr_debug("Thread Stack Info:");
    for (int i = 0; i < kMaxThreadInfo; i++) {
        if (_stackInfo[i].thread_id != 0) {
            tr_debug("\tThread: %d", i);
            tr_debug("\t\tThread Id: 0x%08X with name %s",
                     _stackInfo[i].thread_id, osThreadGetName((osThreadId_t) _stackInfo[i].thread_id));
            tr_debug("\t\tMaximum number of bytes used on the stack: %d", _stackInfo[i].max_size);
            tr_debug("\t\tCurrent number of bytes allocated for the stack: %d", _stackInfo[i].reserved_size);
            tr_debug("\t\tNumber of stacks stats accumulated in the structure: %d", _stackInfo[i].stack_cnt);
        }
    }
}

void MemoryLogger::getAndPrintStatistics()
{
    getAndPrintHeapStatistics();
    getAndPrintStackStatistics();
}

void MemoryLogger::printRuntimeMemoryMap() {
  // defined in rtx_thread.c
  // uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items)
  tr_debug("Runtime Memory Map:");
  osThreadId_t threadIdArray[kMaxThreadInfo] = {0};
  uint32_t nbrOfThreads = osThreadEnumerate(threadIdArray, kMaxThreadInfo);
  for (uint32_t threadIndex = 0; threadIndex < nbrOfThreads; threadIndex++) {
        osRtxThread_t* pThreadCB = (osRtxThread_t*) threadIdArray[threadIndex];
        uint8_t state = pThreadCB->state & osRtxThreadStateMask;
        const char* szThreadState = (state & osThreadInactive) ? "Inactive" : (state & osThreadReady) ? "Ready" :
          (state & osThreadRunning) ? "Running" : (state & osThreadBlocked) ? "Blocked" : (state & osThreadTerminated) ? "Terminated" : "Unknown";
         tr_debug("\t thread with name %s, stack_start: %p, stack_end: %p, size: %u, priority: %d, state: %s",
                  pThreadCB->name, pThreadCB->stack_mem,
                  (char*) pThreadCB->stack_mem + pThreadCB->stack_size,
                  pThreadCB->stack_size,
                  pThreadCB->priority,
                  szThreadState);
  }
  tr_debug("\t mbed_heap_start: %p, mbed_heap_end: %p, size: %u", 
          mbed_heap_start, (mbed_heap_start + mbed_heap_size), mbed_heap_size);
  tr_debug("\t mbed_stack_isr_start: %p, mbed_stack_isr_end: %p, size: %u", mbed_stack_isr_start, (mbed_stack_isr_start + mbed_stack_isr_size), mbed_stack_isr_size);
}

#endif // MBED_ALL_STATS_ENABLED

} // namespace multi_tasking