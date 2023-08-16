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

#pragma once

#include "mbed.h"
#include "multi_tasking/data_types.hpp"

namespace multi_tasking {

class BikeDisplay {
   public:
    explicit BikeDisplay(ProcessedMail& processedMail);  // NOLINT(runtime/references)

    // the LCD display runs its own thread
    void start();

    struct ImageInfo {
        const uint32_t* pImageData;
        const uint8_t imageWidth;
        const uint8_t imageHeight;
    };

    void displayGear(uint8_t gear);

   private:
    // private methods
    void displayInfo();

    // data members
    ProcessedMail& _processedMail;
    Thread _thread;
    static const BikeDisplay::ImageInfo _imageInfo[2];
};

}  // namespace multi_tasking
