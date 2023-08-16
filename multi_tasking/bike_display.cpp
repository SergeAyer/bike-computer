// Copyright 2022 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file bike_display.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike Display header file
 *
 * @date 2023-07-05
 * @version 0.0.1
 ***************************************************************************/

#include "multi_tasking/bike_display.hpp"

// DISCO specific
#include "gear_icon.hpp"
#include "lcd_display.hpp"
#include "mbed_trace.h"
#include "speedometer_icon.hpp"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "BikeDisplay"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace multi_tasking {

// create the LCDDisplay instance as a global variable (not on stack)
static disco::LCDDisplay lcdDisplay;

const BikeDisplay::ImageInfo BikeDisplay::_imageInfo[2] = {
    {.pImageData  = speedometer_icon_100,
     .imageWidth  = kSpeedometerIconWidth,
     .imageHeight = kSpeedometerIconHeight},
    {.pImageData  = gear_icon_80,
     .imageWidth  = kGearIconWidth,
     .imageHeight = kGearIconHeight}};

BikeDisplay::BikeDisplay(ProcessedMail& processedMail)
    : _processedMail(processedMail),
      _thread(osPriorityNormal, OS_STACK_SIZE, nullptr, "LCDDisplay") {}

void BikeDisplay::start() {
    // intitialize the LCD display
    disco::ReturnCode displayRC = lcdDisplay.init();
    if (displayRC != disco::ReturnCode::Ok) {
        tr_error("Failed to initialized display");
    } else {
        tr_debug("Display initialized");
    }

    uint32_t imageIndex = 0;
    // lcdDisplay.displayPicture((uint32_t *)images[imageIndex], 240, 160, 320, 240);
    lcdDisplay.displayPicture(const_cast<uint32_t*>(_imageInfo[imageIndex].pImageData),
                              240,
                              160,
                              _imageInfo[imageIndex].imageWidth,
                              _imageInfo[imageIndex].imageHeight);
    imageIndex = 1;
    lcdDisplay.displayPicture(const_cast<uint32_t*>(_imageInfo[imageIndex].pImageData),
                              340,
                              160,
                              _imageInfo[imageIndex].imageWidth,
                              _imageInfo[imageIndex].imageHeight);

    // start a thread for displaying information
    _thread.start(callback(this, &BikeDisplay::displayInfo));
}

void BikeDisplay::displayGear(uint8_t gear) {
    char msg[10] = {0};
    snprintf(msg, sizeof(msg), "%d", gear);
    tr_debug("Displaying gear %d", gear);
    lcdDisplay.displayStringAtLine(10, msg);
}

void BikeDisplay::displayInfo() {
    while (true) {
        ProcessedData* pProcessedData =
            _processedMail.try_get_for(Kernel::wait_for_u32_forever);

        float speed        = pProcessedData->averageSpeed;
        float rotationRate = pProcessedData->averageRotationRate;

        char msg[256] = {0};
        snprintf(msg,
                 sizeof(msg),
                 "Average speed is %f km/h with rotation rate %f (rotation/sec)\n",
                 speed,
                 rotationRate);
        // printf("%s", msg);
        lcdDisplay.displayStringAtLine(2, msg);
        lcdDisplay.refreshLCD();
        _processedMail.free(pProcessedData);
    }
}

}  // namespace multi_tasking
