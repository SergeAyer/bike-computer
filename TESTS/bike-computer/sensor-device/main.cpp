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
#include "sensor_device.hpp"
#include "unity/unity.h"
#include "utest/utest.h"

using namespace utest::v1;

// test_hdc1000 test handler function
static control_t test_hdc1000(const size_t call_count) {
    // create the hdc1000 sensor instance
    sensors::HDC1000 hdc1000(PD_13, PD_12, PC_6);

    bool present = hdc1000.probe();
    TEST_ASSERT_TRUE(present);

    float temperature                        = hdc1000.getTemperature();
    static constexpr float kTemperatureRange = 20.0f;
    static constexpr float kMeanTemperature  = 15.0f;
    TEST_ASSERT_FLOAT_WITHIN(kTemperatureRange, kMeanTemperature, temperature);

    float humidity                        = hdc1000.getHumidity();
    static constexpr float kHumidityRange = 40.0f;
    static constexpr float kMeanHumidity  = 50.0f;
    TEST_ASSERT_FLOAT_WITHIN(kHumidityRange, kMeanHumidity, humidity);

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

// test_hdc1000 test handler function
static control_t test_sensor_device(const size_t call_count) {
    // create the SensorDevice instance
    common::SensorDevice sensorDevice;

    bool rc = sensorDevice.init();
    TEST_ASSERT_TRUE(rc);

    float temperature                        = sensorDevice.readTemperature();
    static constexpr float kTemperatureRange = 20.0f;
    static constexpr float kMeanTemperature  = 15.0f;
    TEST_ASSERT_FLOAT_WITHIN(kTemperatureRange, kMeanTemperature, temperature);

    float humidity                        = sensorDevice.readHumidity();
    static constexpr float kHumidityRange = 40.0f;
    static constexpr float kMeanHumidity  = 50.0f;
    TEST_ASSERT_FLOAT_WITHIN(kHumidityRange, kMeanHumidity, humidity);

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
static Case cases[] = {Case("test hdc1000 sensor", test_hdc1000),
                       Case("test sensor device", test_sensor_device)};

static Specification specification(greentea_setup, cases);

int main() { return !Harness::run(specification); }
