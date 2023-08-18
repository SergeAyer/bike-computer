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
 * @file consumer.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Class used as a consumer using buffer
 *
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#pragma once

#include "mbed.h"
#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "Producer"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

#include "buffer.hpp"

class Consumer {
   public:
    Consumer(Buffer& buffer,  // NOLINT(runtime/references)
             const std::chrono::microseconds& consumeWaitTime,
             uint32_t nbrOfValues)
        : _buffer(buffer),
          _consumeWaitTime(consumeWaitTime),
          _nbrOfValues(nbrOfValues),
          _consumerThread(osPriorityNormal, OS_STACK_SIZE, nullptr, "ConsumerThread") {
        // initialize random seed
        srand(time(NULL));
    }

    void start() { _consumerThread.start(callback(this, &Consumer::consumerMethod)); }

    void wait() { _consumerThread.join(); }

   private:
    void consume(int datum) {
        // wait for a randomized consumer wait time
        wait_us(computeRandomWaitTime(_consumeWaitTime));
    }

    int computeRandomWaitTime(const std::chrono::microseconds& waitTime) {
        return rand() % waitTime.count() + waitTime.count();
    }

    void consumerMethod(void) {
        uint32_t index = 0;
        while (index < _nbrOfValues) {
            uint32_t consumerDatum = _buffer.extract();
            consume(consumerDatum);
            tr_debug("Extracting data at index %" PRIu32 " (data  %" PRIu32 ")\n",
                     index,
                     consumerDatum);
            index++;
        }
    }

   private:
    Buffer& _buffer;
    const std::chrono::microseconds& _consumeWaitTime;
    uint32_t _nbrOfValues;
    Thread _consumerThread;
};
