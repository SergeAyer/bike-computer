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
 * @file bike_system.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Task logger (for times) header file
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

namespace utils {

class TaskLogger {
   public:
    void enable(bool enable);
    void logPeriodAndExecutionTime(Timer& timer,  // NOLINT(runtime/references)
                                   int taskIndex,
                                   const std::chrono::microseconds& taskStartTime);

    static constexpr int kResetTaskIndex   = 0;
    static constexpr int kGearTaskIndex    = 1;
    static constexpr int kCountTaskIndex   = 2;
    static constexpr int kDisplayTaskIndex = 3;
    static constexpr int kSensorTaskIndex  = 4;

   private:
    // constants
    static constexpr int kNbrOfTasks          = 5;
    static constexpr char* kTaskDescriptors[] = {const_cast<char*>("Reset"),
                                                 const_cast<char*>("Gear"),
                                                 const_cast<char*>("Count"),
                                                 const_cast<char*>("Display"),
                                                 const_cast<char*>("Sensor")};

    // data members
    bool _isEnabled = true;
    std::chrono::microseconds _taskStartTime[kNbrOfTasks];
};

}  // namespace utils
