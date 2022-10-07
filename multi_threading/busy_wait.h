#pragma once

inline void busy_wait_ms(uint32_t ms)  
{
#if defined(NDEBUG)
    static constexpr uint64_t kNbrOfIterationsFor1Ms = 8520;
#else
    static constexpr uint64_t kNbrOfIterationsFor1Ms = 10852;
    
#endif
    const uint64_t kNbrOfIterations = ms * kNbrOfIterationsFor1Ms;
    for (uint64_t i = 0; i < kNbrOfIterations; i++) {
        asm("nop");
    }
}