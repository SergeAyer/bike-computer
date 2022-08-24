#include "lcd_display.hpp"

namespace static_scheduling {

LCDDisplay::LCDDisplay()
{
}

void LCDDisplay::show(int currentGear, int currentRotationCount, int subTaskIndex)
{
    if (subTaskIndex == -1) {
        // simulate task computation by waiting for the required task run time
        wait_us(kTaskRunTime.count());
        // use printf since we want to print in any case
        printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
    } else {
        // simulate task computation by waiting for the required task run time
        wait_us(kSubTaskRunTime.count());
        if (subTaskIndex == (kNbrOfSubTasks - 1)) {
            // use printf since we want to print in any case
            printf("Gear value is %d, wheel rotation count is %d\n", currentGear, currentRotationCount);
        }
    }
}

} // namespace static_scheduling
