#pragma once


#include "update-client/candidate_applications.hpp"

namespace bike_computer {

class MyCandidateApplications : public update_client::CandidateApplications {
public:
    MyCandidateApplications(BlockDevice &blockDevice,
                            uint32_t storageAddress, uint32_t storageSize,
                            uint32_t headerSize, uint32_t nbrOfSlots);

    virtual uint32_t getSlotForCandidate() override;
};

}  // namespace bike_computer


update_client::CandidateApplications *createCandidateApplications(BlockDevice &blockDevice,
                                                                  uint32_t storageAddress,
                                                                  uint32_t storageSize,
                                                                  uint32_t headerSize,
                                                                  uint32_t nbrOfSlots);
