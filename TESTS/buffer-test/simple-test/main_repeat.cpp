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
 * @brief Demo example of test program for demonstrating test run multiple times
 *
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#include "buffer.hpp"
#include "consumer.hpp"
#include "greentea-client/test_env.h"
#include "mbed.h"
#include "producer.hpp"
#include "unity/unity.h"
#include "utest/utest.h"

using namespace utest::v1;

/** Test production and consumption of a given number of values
 *
 * Given a buffer, create a producer and consumer thread and make sure that
 * both threads return after a given number of values have been produced and consumed-
 */

static status_t check_producer_consumer_setup(const Case* const source,
                                              const size_t index_of_case) {
    // Call the default handler for proper reporting
    status_t status = greentea_case_setup_handler(source, index_of_case);
    printf("Setting up for '%s' (status %d)\n", source->get_description(), status);
    return status;
}

static control_t check_producer_consumer(const size_t call_count) {
    printf("Called for the %u. time\n", call_count);
    // create a buffer
    Buffer buffer;
    // nbr of values to be produced/consumed for the test
    static constexpr uint32_t nbrOfValues = 30;
    // create a producer and a consumer
    static constexpr std::chrono::microseconds kProduceWaitTime[] = {
        500000us, 50000us, 500000us};
    Producer producer(buffer, kProduceWaitTime[call_count - 1], nbrOfValues);
    static constexpr std::chrono::microseconds kConsumeWaitTime[] = {
        500000us, 500000us, 50000us};
    Consumer consumer(buffer, kConsumeWaitTime[call_count - 1], nbrOfValues);
    // start both
    producer.start();
    consumer.start();

    // wait for threads to terminate
    producer.wait();
    consumer.wait();

    // Specify how often this test is repeated ie. n total calls
    static constexpr uint8_t kNbrOfCalls = 3;
    return (call_count < kNbrOfCalls) ? CaseRepeatAll : CaseNext;
}

static utest::v1::status_t greentea_setup(const size_t number_of_cases) {
    // Here, we specify the timeout (60s) and the host test (a built-in host test or the
    // name of our Python file)
    GREENTEA_SETUP(120, "default_auto");

    return greentea_test_setup_handler(number_of_cases);
}

// List of test cases in this file
static Case cases[] = {Case(
    "producer consumer test", check_producer_consumer_setup, check_producer_consumer)};

static Specification specification(greentea_setup, cases);

int main() { return !Harness::run(specification); }
