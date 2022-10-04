#pragma once

#include "multi_tasking/data_types.hpp"

#include "mbed.h"

namespace multi_tasking {

class LCDDisplay {
public:
    LCDDisplay(ProcessedMail &processedMail);

    // the LCD display runs its own thread
    void start();

private:
    // private methods
    void displayInfo();

    // data members
    ProcessedMail &_processedMail;
    Thread _thread;
};

} // namespace multi_tasking
