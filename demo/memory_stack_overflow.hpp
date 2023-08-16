#pragma once

#include <cstdint>

#include "mbed.h"

namespace multi_tasking {

class MemoryStackOverflow {
   public:
    void allocateOnStack() {
        // allocate an array with growing size until it does not fit on the stack anymore
        size_t allocSize = kArraySize * _multiplier;
        // Create a variable-size object on the stack
        double anotherArray[allocSize];
        for (size_t i = 0; i < allocSize; i++) {
            anotherArray[i] = i;
        }
        // copy to member variable to prevent them from being optimized away
        for (size_t i = 0; i < kArraySize; i++) {
            _doubleArray[i] += anotherArray[i];
        }
        _multiplier++;
    }

   private:
    static constexpr size_t kArraySize = 40;
    double _doubleArray[kArraySize]    = {0};
    size_t _multiplier                 = 1;
};

}  // namespace multi_tasking
