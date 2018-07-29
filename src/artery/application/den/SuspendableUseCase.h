#ifndef ARTERY_DEN_SUSPENDABLEUSECASE_H_P2OWWY0L
#define ARTERY_DEN_SUSPENDABLEUSECASE_H_P2OWWY0L

#include "artery/application/den/UseCase.h"
#include <boost/optional.hpp>
#include <omnetpp/simtime.h>

namespace artery
{
namespace den
{

class SuspendableUseCase : public UseCase
{
public:
    void initialize(int) override;

protected:
    void setDetectionBlockingInterval(omnetpp::SimTime);
    void blockDetection();
    bool isDetectionBlocked();

private:
    omnetpp::SimTime mDetectionBlockingInterval;
    boost::optional<omnetpp::SimTime> mDetectionBlockingSince;
};

} // namespace den
} // namespace artery

#endif /* ARTERY_DEN_SUSPENDABLEUSECASE_H_P2OWWY0L */

