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
 * @brief ResetDevice implementation (multi-tasking)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "reset_device.hpp"

#if defined(TARGET_DISCO_H747I)
#define PUSH_BUTTON BUTTON1
static constexpr uint8_t kPolarityPressed = 1;
#endif

namespace multi_tasking {

ResetDevice::ResetDevice(Callback<void()> cb) : _resetButton(PUSH_BUTTON) {
    // register a callback for computing the response time
    _resetButton.fall(cb);
}

}  // namespace multi_tasking
