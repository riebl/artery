#include "artery/application/den/SuspendableUseCase.h"

namespace artery
{
namespace den
{

void SuspendableUseCase::initialize(int stage)
{
    UseCase::initialize(stage);
    if (stage == 0) {
        setDetectionBlockingInterval(par("detectionBlockingInterval"));
    }
}

void SuspendableUseCase::setDetectionBlockingInterval(omnetpp::SimTime block)
{
    mDetectionBlockingInterval = std::max(omnetpp::SimTime::ZERO, block);
}

bool SuspendableUseCase::isDetectionBlocked()
{
    if (mDetectionBlockingSince) {
        const auto now = omnetpp::simTime();
        if (*mDetectionBlockingSince + mDetectionBlockingInterval < now) {
            mDetectionBlockingSince.reset();
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

void SuspendableUseCase::blockDetection()
{
    mDetectionBlockingSince = omnetpp::simTime();
}

} // namespace den
} // namespace artery
