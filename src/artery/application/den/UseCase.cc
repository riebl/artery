#include "artery/application/den/UseCase.h"
#include "artery/application/DenService.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/application/VehicleDataProvider.h"
#include <boost/units/systems/si/prefixes.hpp>
#include <omnetpp/checkandcast.h>

namespace artery
{
namespace den
{

static const auto microdegree = vanetza::units::degree * boost::units::si::micro;
static const auto decidegree = vanetza::units::degree * boost::units::si::deci;
static const auto centimeter_per_second = vanetza::units::si::meter_per_second * boost::units::si::centi;

template<typename T, typename U>
long round(const boost::units::quantity<T>& q, const U& u)
{
    boost::units::quantity<U> v { q };
    return std::round(v.value());
}

void UseCase::initialize(int stage)
{
    if (stage == 0) {
        mService = omnetpp::check_and_cast<DenService*>(getParentModule());
        mVdp = &mService->getFacilities().get_const<VehicleDataProvider>();
    }
}

vanetza::asn1::Denm UseCase::createMessageSkeleton()
{
    vanetza::asn1::Denm message;
    message->header.protocolVersion = 1;
    message->header.messageID = ItsPduHeader__messageID_denm;
    message->header.stationID = mVdp->station_id();

    // Do not copy ActionID itself (it also contains a context object)
    auto action_id = mService->requestActionID();
    message->denm.management.actionID.originatingStationID = action_id.originatingStationID;
    message->denm.management.actionID.sequenceNumber = action_id.sequenceNumber;
    int ret = 0;
    const auto taiTime = countTaiMilliseconds(mService->getTimer()->getTimeFor(mVdp->updated()));
    ret += asn_long2INTEGER(&message->denm.management.detectionTime, taiTime);
    ret += asn_long2INTEGER(&message->denm.management.referenceTime, taiTime);
    assert(ret == 0);
    message->denm.management.eventPosition.altitude.altitudeValue = AltitudeValue_unavailable;
    message->denm.management.eventPosition.altitude.altitudeConfidence = AltitudeConfidence_unavailable;
    message->denm.management.eventPosition.longitude = round(mVdp->longitude(), microdegree) * Longitude_oneMicrodegreeEast;
    message->denm.management.eventPosition.latitude = round(mVdp->latitude(), microdegree) * Latitude_oneMicrodegreeNorth;
    message->denm.management.eventPosition.positionConfidenceEllipse.semiMajorOrientation = HeadingValue_unavailable;
    message->denm.management.eventPosition.positionConfidenceEllipse.semiMajorConfidence = SemiAxisLength_unavailable;
    message->denm.management.eventPosition.positionConfidenceEllipse.semiMinorConfidence = SemiAxisLength_unavailable;

    message->denm.location = vanetza::asn1::allocate<LocationContainer_t>();
    message->denm.location->eventSpeed = vanetza::asn1::allocate<Speed>();
    message->denm.location->eventSpeed->speedValue = std::abs(round(mVdp->speed(), centimeter_per_second)) * SpeedValue_oneCentimeterPerSec;
    message->denm.location->eventSpeed->speedConfidence = SpeedConfidence_equalOrWithinOneCentimeterPerSec * 3;
    message->denm.location->eventPositionHeading = vanetza::asn1::allocate<Heading>();
    message->denm.location->eventPositionHeading->headingValue = round(mVdp->heading(), decidegree);
    message->denm.location->eventPositionHeading->headingConfidence = HeadingConfidence_equalOrWithinOneDegree;

    // TODO fill path history
    auto path_history = vanetza::asn1::allocate<PathHistory_t>();
    asn_sequence_add(&message->denm.location->traces, path_history);

    return message;
}

} // namespace den
} // namespace artery
