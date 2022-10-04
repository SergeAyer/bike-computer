#include "static_scheduling_with_event/lcd_display.hpp"

namespace static_scheduling_with_event {

const std::chrono::microseconds LCDDisplay::kSubTaskRunTime[kNbrOfSubTasks] = { 200000us, 100000us };

LCDDisplay::LCDDisplay()
{
}

void LCDDisplay::show(uint8_t currentGear, uint32_t currentRotationCount, uint8_t subTaskIndex)
{
    if (subTaskIndex == UINT8_MAX) {
        // simulate task computation by waiting for the required task run time
        wait_us(kTaskRunTime.count());
        // use printf since we want to print in any case
        printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
    } 
    else {
        // simulate task computation by waiting for the required task run time
        wait_us(kSubTaskRunTime[subTaskIndex].count());
        if (subTaskIndex == (kNbrOfSubTasks - 1)) {
            // use printf since we want to print in any case
            printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
        }
    }
}

} // namespace static_scheduling_with_event
