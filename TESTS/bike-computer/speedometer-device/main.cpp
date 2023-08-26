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
 * @brief Bike computer test suite: speedometer device
 *
 * @date 2023-08-26
 * @version 0.1.0
 ***************************************************************************/

#include <chrono>

#include "greentea-client/test_env.h"
#include "mbed.h"
#include "static_scheduling/gear_system_device.hpp"
#include "static_scheduling/speedometer_device.hpp"
#include "unity/unity.h"
#include "utest/utest.h"

using namespace utest::v1;

// function called by test handler functions for verifying the current speed

void test_current_speed(const std::chrono::milliseconds& pedalRotationTime,
                        uint8_t traySize,
                        uint8_t gearSize,
                        float wheelCircumference,
                        float currentSpeed) {
    // compute the number of pedal rotation per hour
    uint32_t milliSecondsPerHour = 1000 * 3600;
    float pedalRotationsPerHour  = static_cast<float>(milliSecondsPerHour) /
                                  static_cast<float>(pedalRotationTime.count());

    // compute the expected speed in km / h
    // first compute the distance in meter for each pedal turn
    float trayGearRatio = static_cast<float>(traySize) / static_cast<float>(gearSize);
    float distancePerPedalTurn = trayGearRatio * wheelCircumference;
    float expectedSpeed        = (distancePerPedalTurn / 1000.0f) * pedalRotationsPerHour;

    printf("  Expected speed is %f, current speed is %f\n", expectedSpeed, currentSpeed);
    static constexpr float kAllowedDelta = 0.1f;
    TEST_ASSERT_FLOAT_WITHIN(kAllowedDelta, expectedSpeed, currentSpeed);
}

// test the speedometer by modifying the gear
static control_t test_gear(const size_t call_count) {
    // this is the always succeed test
    Timer timer;
    // start the timer
    timer.start();

    // create a speedometer instance
    static_scheduling::SpeedometerDevice speedometer(timer);

    // let the speedometer thread start properly
    ThisThread::sleep_for(
        static_scheduling::SpeedometerDevice::kInitialPedalRotationTime *
        static_scheduling::SpeedometerDevice::kInitialRotationCount);

    // get speedometer constant values (for this test)
    const uint8_t traySize         = speedometer.getTraySize();
    const float wheelCircumference = speedometer.getWheelCircumference();
    const std::chrono::milliseconds pedalRotationTime =
        speedometer.getCurrentPedalRotationTime();

    for (uint8_t gear = static_scheduling::GearSystemDevice::kMinGear;
         gear <= static_scheduling::GearSystemDevice::kMaxGear;
         gear++) {
        // set the gear
        printf("Testing gear %d\n", gear);
        speedometer.setGear(gear);

        // let the speedometer update the speed
        ThisThread::sleep_for(pedalRotationTime);

        // get the current speed
        float currentSpeed = speedometer.getCurrentSpeed();

        // test it
        uint8_t gearSize = speedometer.getGearSize();
        test_current_speed(
            pedalRotationTime, traySize, gearSize, wheelCircumference, currentSpeed);
    }

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

// test the speedometer by modifying the pedal rotation speed
static control_t test_rotation_speed(const size_t call_count) {
    // this is the always succeed test
    Timer timer;
    // start the timer
    timer.start();

    // create a speedometer instance
    static_scheduling::SpeedometerDevice speedometer(timer);

    // set the gear
    speedometer.setGear(static_scheduling::GearSystemDevice::kMinGear);

    // let the speedometer thread start properly
    ThisThread::sleep_for(
        static_scheduling::SpeedometerDevice::kInitialPedalRotationTime *
        static_scheduling::SpeedometerDevice::kInitialRotationCount);

    // get speedometer constant values
    const uint8_t traySize         = speedometer.getTraySize();
    const float wheelCircumference = speedometer.getWheelCircumference();
    const uint8_t gearSize         = speedometer.getGearSize();

    // first test increasing rotation speed (decreasing rotation time)
    std::chrono::milliseconds pedalRotationTime =
        speedometer.getCurrentPedalRotationTime();
    while (pedalRotationTime >
           static_scheduling::SpeedometerDevice::kMinPedalRotationTime) {
        // increase the pedal rotation time
        speedometer.increaseRotationSpeed();

        // let the speedometer update the speed
        ThisThread::sleep_for(pedalRotationTime);

        // if the rotation speed is left unchanged, then we reached the largest rotation
        // time
        pedalRotationTime = speedometer.getCurrentPedalRotationTime();

        // get the current speed
        float currentSpeed = speedometer.getCurrentSpeed();

        // test it
        test_current_speed(
            pedalRotationTime, traySize, gearSize, wheelCircumference, currentSpeed);
    }

    // second test decreasing rotation speed (increasing rotation time)
    pedalRotationTime = speedometer.getCurrentPedalRotationTime();
    while (pedalRotationTime <
           static_scheduling::SpeedometerDevice::kMaxPedalRotationTime) {
        // decrease the pedal rotation time
        speedometer.decreaseRotationSpeed();

        // let the speedometer update the speed
        ThisThread::sleep_for(pedalRotationTime);

        // if the rotation speed is left unchanged, then we reached the largest rotation
        // time
        pedalRotationTime = speedometer.getCurrentPedalRotationTime();

        // get the current speed
        float currentSpeed = speedometer.getCurrentSpeed();

        // test it
        test_current_speed(
            pedalRotationTime, traySize, gearSize, wheelCircumference, currentSpeed);
    }

    // execute the test only once and move to the next one, without waiting
    return CaseNext;
}

static utest::v1::status_t greentea_setup(const size_t number_of_cases) {
    // Here, we specify the timeout (60s) and the host test (a built-in host test or the
    // name of our Python file)
    GREENTEA_SETUP(120, "default_auto");

    return greentea_test_setup_handler(number_of_cases);
}

// List of test cases in this file
static Case cases[] = {
    Case("test speedometer gear change", test_gear),
    Case("test speedometer rotation speed change", test_rotation_speed)};

static Specification specification(greentea_setup, cases);

int main() { return !Harness::run(specification); }
