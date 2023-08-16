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
 * @file reset_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Reset device implementation
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#include "multi_tasking/reset_device.hpp"

#if defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F746NG) || \
    defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
#define POLARITY_PRESSED 0
#endif

namespace multi_tasking {

ResetDevice::ResetDevice(ResetDeviceCallback cb) : _resetButton(PUSH_BUTTON) {
    // register a callback for computing the response time
    _resetButton.fall(cb);
}

}  // namespace multi_tasking
