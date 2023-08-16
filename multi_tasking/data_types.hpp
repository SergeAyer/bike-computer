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
 * @file data_types.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Definition of data types used for exchanging data
 *
 * @date 2022-07-05
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

namespace multi_tasking {

struct ProcessedData {
    float averageSpeed        = 0;
    float averageRotationRate = 0;
};

static constexpr uint32_t kCountQueueSize = 32;
typedef Queue<uint32_t, kCountQueueSize> CountQueue;

static constexpr uint32_t kProcessedDataMailSize = 32;
typedef Mail<ProcessedData, kProcessedDataMailSize> ProcessedMail;

}  // namespace multi_tasking
