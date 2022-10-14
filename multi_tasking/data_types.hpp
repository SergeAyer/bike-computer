#pragma once

#include "mbed.h"

namespace multi_tasking {

struct ProcessedData {
    float averageSpeed = 0;
    float averageRotationRate = 0;
};

static constexpr uint32_t kCountQueueSize = 32;
typedef Queue<uint32_t, kCountQueueSize> CountQueue;

static constexpr uint32_t kProcessedDataMailSize = 32;
typedef Mail<ProcessedData, kProcessedDataMailSize> ProcessedMail;


} // namespace multi_tasking