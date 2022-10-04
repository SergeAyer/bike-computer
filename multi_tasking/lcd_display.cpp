#include "multi_tasking/lcd_display.hpp"

namespace multi_tasking {

LCDDisplay::LCDDisplay(ProcessedMail &processedMail) : 
    _processedMail(processedMail),
    _thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "LCDDisplay")
{
}

void LCDDisplay::start() 
{
    // start a thread for displaying information
  _thread.start(callback(this, &LCDDisplay::displayInfo));
}

void LCDDisplay::displayInfo()
{
    while (true) {
        ProcessedData* pProcessedData = _processedMail.try_get_for(Kernel::wait_for_u32_forever);
        
        float speed = pProcessedData->averageSpeed;
        float power = pProcessedData->averagePower;
    
        printf("Average speed is %f with power %f\n", speed, power);
        _processedMail.free(pProcessedData);    
    }    
}

} // namespace multi_tasking
