#include "artery/traci/Cast.h"
#include "artery/traci/PersonMobility.h"
#include <omnetpp/cwatch.h>

using namespace traci;

namespace artery
{

void PersonMobility::initializeSink(std::shared_ptr<API> api, std::shared_ptr<PersonCache> cache, const Boundary& boundary)
{
    ASSERT(api);
    ASSERT(cache);
    mTraci = api;
    mPersonId = cache->getId();
    mNetBoundary = boundary;
    mController.reset(new PersonController(api, cache));
}

void PersonMobility::initializePerson(const TraCIPosition& traci_pos, TraCIAngle traci_heading, double traci_speed)
{
    const auto opp_pos = position_cast(mNetBoundary, traci_pos);
    const auto opp_angle = angle_cast(traci_heading);
    initialize(opp_pos, opp_angle, traci_speed);
}

void PersonMobility::updatePerson(const TraCIPosition& traci_pos, TraCIAngle traci_heading, double traci_speed)
{
    const auto opp_pos = position_cast(mNetBoundary, traci_pos);
    const auto opp_angle = angle_cast(traci_heading);
    update(opp_pos, opp_angle, traci_speed);
}

PersonController* PersonMobility::getPersonController()
{
    ASSERT(mController);
    return mController.get();
}

} // namespace artery
