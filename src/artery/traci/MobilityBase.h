#ifndef ARTERY_MOBILITYBASE_H_1SQMAVHF
#define ARTERY_MOBILITYBASE_H_1SQMAVHF

#include "traci/API.h"
#include "artery/utility/Geometry.h"
#include <omnetpp/ccomponent.h>
#include <memory>

namespace artery
{

class MobilityBase
{
public:
    // generic signal for mobility state changes
    static omnetpp::simsignal_t stateChangedSignal;

protected:
    virtual void initialize(const Position&, Angle, double speed) = 0;
    virtual void update(const Position&, Angle, double speed) = 0;

    std::shared_ptr<traci::API> mTraci;
    traci::Boundary mNetBoundary;
};

} // namespace artery

#endif /* ARTERY_MOBILITYBASE_H_1SQMAVHF */
