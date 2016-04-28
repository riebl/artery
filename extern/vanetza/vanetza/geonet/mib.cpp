#include "mib.hpp"
#include <boost/units/systems/si/prefixes.hpp>

namespace vanetza
{
namespace geonet
{

using namespace vanetza::units::si;
using vanetza::units::degrees;
using boost::units::si::kilo;
using boost::units::si::milli;

const auto milliseconds = milli * seconds;

ManagementInformationBase::ManagementInformationBase() :
    itsGnLocalAddrConfMethod(AddrConfMethod::MANAGED),
    itsGnProtocolVersion(0), // TS 102 636-4-1 (V1.1.1) = 0
    itsGnStationType(StationType::PASSENGER_CAR),
    itsGnIsMobile(true),
    itsGnIfType(InterfaceType::ITS_G5),
    itsGnMinimumUpdateFrequencyLPV(1.0 / (1000.0 * milliseconds)),
    itsGnPaiInterval(10 * meters),
    itsGnMaxSduSize(1398),
    itsGnMaxGeoNetworkingHeaderSize(88),
    itsGnLifetimeLocTE(20 * seconds),
    itsGnSecurity(false),
    itsGnSnDecapResultHandling(SecurityDecapHandling::STRICT),
    itsGnLocationServiceMaxRetrans(10),
    itsGnLocationServiceRetransmitTimer(1 * seconds),
    itsGnLocationServicePacketBufferSize(1024),
    itsGnBeaconServiceRetransmitTimer(3 * seconds),
    itsGnBeaconServiceMaxJitter(itsGnBeaconServiceRetransmitTimer / 4.0),
    itsGnDefaultHopLimit(10),
    itsGnMaxPacketLifetime(Lifetime::Base::_100_S, 6),
    itsGnDefaultPacketLifetime(Lifetime::Base::_10_S, 6),
    itsGnMaxPacketDataRate(100),
    itsGnMaxGeoAreaSize(10 * kilo * kilo * square_meters),
    itsGnMinPacketRepetitionInterval(100 * milliseconds),
    itsGnGeoUnicastForwardingAlgorithm(UnicastForwarding::GREEDY),
    itsGnGeoBroadcastForwardingAlgorithm(BroadcastForwarding::ADVANCED),
    itsGnGeoUnicastCbfMinTime(1 * milliseconds),
    itsGnGeoUnicastCbfMaxTime(100 * milliseconds),
    itsGnGeoBroadcastCbfMinTime(1 * milliseconds),
    itsGnGeoBroadcastCbfMaxTime(100 * milliseconds),
    itsGnDefaultMaxCommunicationRange(1000 * meters),
    itsGnBroadcastCBFDefSectorAngle(30 * degrees),
    itsGnUnicastCBFDefSectorAngle(30 * degrees),
    itsGnGeoAreaLineForwarding(true),
    itsGnUcForwardingPacketBufferSize(256),
    itsGnBcForwardingPacketBufferSize(1024),
    itsGnCbfPacketBufferSize(256),
    itsGnDefaultTrafficClass(false, false, 0)
{
}

} // namespace geonet
} // namespace vanetza

