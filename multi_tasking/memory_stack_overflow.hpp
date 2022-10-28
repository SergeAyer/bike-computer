#pragma once

#include "mbed.h"
#include <cstdint>

namespace multi_tasking {

class MemoryStackOverflow {
public:
    void allocateOnStack()
    {
        // allocate an array with growing size until it does not fit on the stack anymore
        uint32_t allocSize = kArraySize  * _multiplier;
        // Create a variable-size object on the stack
        double anotherArray[allocSize];
        for (size_t ix = 0; ix < allocSize; ix++) {
            anotherArray[ix] = ix;
        }
        // copy to member variable to prevent them from being optimized away
        for (int i = 0; i < kArraySize; i++) {
            _doubleArray[i] += anotherArray[i];
        }
        //_multiplier++;
    }

private:
    static constexpr uint32_t kArraySize = 40;
    double _doubleArray[kArraySize] = {0};
    uint32_t _multiplier = 1;    
};

} // namespace multi_tasking