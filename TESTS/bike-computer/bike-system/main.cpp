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
 * @brief Bike computer test suite: sensor device
 *
 * @date 2023-08-26
 * @version 0.1.0
 ***************************************************************************/

#include "greentea-client/test_env.h"
#include "hdc1000.hpp"
#include "mbed.h"
#include "bike_system.hpp"
#include "task_logger.hpp"
#include "unity/unity.h"
#include "utest/utest.h"
#include <chrono>

using namespace utest::v1;

// test_hdc1000 test handler function
static control_t test_bike_system(const size_t call_count) {
    // create the BikeSystem instance
    static_scheduling::BikeSystem bikeSystem;

    // run the bike system in a separate thread
    Thread thread;
    thread.start(callback(&bikeSystem, &static_scheduling::BikeSystem::start));
    
    // let the bike system run for 20 secs
    ThisThread::sleep_for(20s);

    // stop the bike system
    bikeSystem.stop();

    // check whether scheduling was correct
    constexpr std::chrono::microseconds taskComputationTimes[] = { 100000us, 200000us, 100000us, 100000us, 200000us, 100000us};
    constexpr std::chrono::microseconds taskPeriods[] = { 800000us, 400000us, 1600000us, 800000us, 1600000us, 1600000us};

    // allow for 1 msec offset
    uint64_t deltaUs = 1000;
    for (uint8_t taskIndex = 0; taskIndex < advembsof::TaskLogger::kNbrOfTasks; taskIndex++) {
          TEST_ASSERT_UINT64_WITHIN(deltaUs, 
                                    taskPeriods[taskIndex].count(), 
                                    bikeSystem.getTaskLogger().getPeriod(taskIndex).count());
          TEST_ASSERT_UINT64_WITHIN(deltaUs, 
                                    taskComputationTimes[taskIndex].count(), 
                                    bikeSystem.getTaskLogger().getComputationTime(taskIndex).count());
    }

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

static utest::v1::status_t greentea_setup(const size_t number_of_cases) {
    // Here, we specify the timeout (60s) and the host test (a built-in host test or the
    // name of our Python file)
    GREENTEA_SETUP(60, "default_auto");

    return greentea_test_setup_handler(number_of_cases);
}

// List of test cases in this file
static Case cases[] = { Case("test bike system", test_bike_system) };

static Specification specification(greentea_setup, cases);

int main() { return !Harness::run(specification); }
