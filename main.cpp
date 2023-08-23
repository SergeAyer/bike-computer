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
 * @file main.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief main function for bike computer program
 *
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#include "mbed.h"
#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "main"
#endif  //  MBED_CONF_MBED_TRACE_ENABLE

#include "FlashIAPBlockDevice.h"
// #include "multi_tasking/bike_system.hpp"
// #include "multi_threading/bike_system.hpp"
#include "static_scheduling/bike_system.hpp"
// #include "static_scheduling_with_event/bike_system.hpp"
#include "update-client/usb_serial_uc.hpp"

// main() runs in its own thread in the OS
#if !MBED_TEST_MODE

int main() {
    // EventRecorderInitialize(EventRecordAll, 1);
    //  pb = static_cast<Base*>(&u);
    //  use trace library for console output
    mbed_trace_init();

    tr_debug("Program started");

#if (USE_USB_SERIAL_UC == 1) && defined(HEADER_ADDR)
    // launch the updater using a FlashIAPBlockDevice
    FlashIAPBlockDevice flashIAPBlockDevice(MBED_ROM_START, MBED_ROM_SIZE);
    update_client::USBSerialUC usbSerialUpdateClient(flashIAPBlockDevice);
    update_client::UCErrorCode rc = usbSerialUpdateClient.start();
    if (rc != update_client::UCErrorCode::UC_ERR_NONE) {
        tr_error("Cannot initialize update client: %d", rc);
    } else {
        tr_info("Update client started");
    }
#endif

    // declare the BikeSystem instance
    static_scheduling::BikeSystem bikeSystem;
    // static_scheduling_with_event::BikeSystem bikeSystem;
    // multi_threading::BikeSystem bikeSystem;
    // multi_tasking::BikeSystem bikeSystem;

    // will start the BikeSystem and never return
    tr_debug("Starting the bike system (2)");
    bikeSystem.start();
    while (true) {
    }
}

/**
 * @brief  End of Refresh DSI callback.
 * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
 *               the configuration information for the DSI.
 * @retval None
 */

void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef* hdsi) {
    // tr_debug("Crash");
}

#endif  //! MBED_TEST_MODE
