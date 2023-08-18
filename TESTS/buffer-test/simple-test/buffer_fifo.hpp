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
 * @file buffer_fifo.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Class used for implementing a basic buffer using Semaphore/Mutex
 *
 * @date 2022-09-01
 * @version 0.1.0
 ***************************************************************************/

#pragma once

class Buffer {
   public:
    Buffer() {}

    void append(uint32_t datum) {
        // check that we can produce one element
        _inSemaphore.acquire();

        // enter critical section
        _producerConsumerMutex.lock();

        // produce the data
        _buffer[_appendIndex % kBufferSize] = datum;
        _appendIndex++;

        // exit critical section
        _producerConsumerMutex.unlock();

        // tell that we produced one element
        _outSemaphore.release();
    }

    uint32_t extract(void) {
        // check that we can consume one element
        _outSemaphore.acquire();

        // enter critical section
        _producerConsumerMutex.lock();

        // consumer one element
        uint32_t datum = _buffer[_extractIndex % kBufferSize];
        _extractIndex++;

        // exit critical section
        _producerConsumerMutex.unlock();

        // tell that we consumed one element
        _inSemaphore.release();

        return datum;
    }

   private:
    static constexpr uint8_t kBufferSize = 10;

    uint32_t _buffer[kBufferSize] = {0};
    uint32_t _appendIndex         = 0;
    uint32_t _extractIndex        = 0;
    Mutex _producerConsumerMutex;
    Semaphore _outSemaphore{0};
    Semaphore _inSemaphore{kBufferSize - 1};
};
