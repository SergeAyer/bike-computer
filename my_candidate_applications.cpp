#include "my_candidate_applications.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "MyCandidateApplications"
#endif // MBED_CONF_MBED_TRACE_ENABLE

update_client::CandidateApplications *createCandidateApplications(BlockDevice &blockDevice,
                                                                  uint32_t storageAddress,
                                                                  uint32_t storageSize,
                                                                  uint32_t headerSize,
                                                                  uint32_t nbrOfSlots)
{
    tr_debug("##################### createCandidateApplications");
    return new bike_computer::MyCandidateApplications(blockDevice, storageAddress, storageSize, headerSize, nbrOfSlots);
}

namespace bike_computer {


MyCandidateApplications::MyCandidateApplications(BlockDevice &blockDevice,
                                                 uint32_t storageAddress,
                                                 uint32_t storageSize,
                                                 uint32_t headerSize,
                                                 uint32_t nbrOfSlots) :
    update_client::CandidateApplications(blockDevice, storageAddress, storageSize, headerSize, nbrOfSlots)
{

}

uint32_t MyCandidateApplications::getSlotForCandidate()
{
    tr_debug("##################### getSlotForCandidate");
    // if any of the slot is not occupied by a valid firmware, use it
    uint32_t nbrOfSlots = getNbrOfSlots();
    for (uint32_t slotIndex = 0; slotIndex < nbrOfSlots; slotIndex++) {
        if (! getBlockDeviceApplication(slotIndex).isValid()) {
            return slotIndex;
        }
    }

    // return the slot of the oldest firmware candidates
    uint32_t oldestSlotIndex = 0;
    uint64_t oldestFirmwareVersion = getBlockDeviceApplication(oldestSlotIndex).getFirmwareVersion();
    for (uint32_t slotIndex = 1; slotIndex < nbrOfSlots; slotIndex++) {
        mbed::bd_addr_t candidateAddress = 0;
        mbed::bd_size_t slotSize = 0;
        getCandidateAddress(slotIndex, candidateAddress, slotSize);        
        tr_debug("Checking application at slot %" PRIu32 " (address 0x%" PRIx64 ", size %" PRIu64 ")", 
                 slotIndex, candidateAddress, slotSize);

        if (getBlockDeviceApplication(slotIndex).getFirmwareVersion() < oldestFirmwareVersion) {
            oldestSlotIndex = slotIndex;
            oldestFirmwareVersion = getBlockDeviceApplication(slotIndex).getFirmwareVersion();
        }
    }
    return oldestSlotIndex;
}

} // namespace bike_computer
