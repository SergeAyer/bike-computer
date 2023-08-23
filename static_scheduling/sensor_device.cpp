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
 * @file bike_system.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief SensorDevice implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "static_scheduling/sensor_device.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "SensorDevice"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace static_scheduling {

SensorDevice::SensorDevice() : _hdc1000(PD_13, PD_12, PC_6) {}

bool SensorDevice::init() {
    // probe for testing the presence of the sensor
    bool rc = _hdc1000.probe();
    if (!rc) {
        tr_error("HDC1000 not present");
    }
    return rc;
}

float SensorDevice::readTemperature() { return _hdc1000.getTemperature(); }

}  // namespace static_scheduling
