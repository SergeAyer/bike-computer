#pragma once

#include "mbed.h"

namespace multi_tasking {

struct ProcessedData {
    float averageSpeed = 0;
    float averagePower = 0;
};

static constexpr uint32_t COUNT_QUEUE_SIZE = 32;
typedef Queue<uint32_t, COUNT_QUEUE_SIZE> CountQueue;

static constexpr uint32_t PROCESSED_DATA_MAIL_SIZE = 32;
typedef Mail<ProcessedData, PROCESSED_DATA_MAIL_SIZE> ProcessedMail;


} // namespace multi_tasking