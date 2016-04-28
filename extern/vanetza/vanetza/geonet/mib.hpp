#ifndef MIB_HPP_U3WJ4WES
#define MIB_HPP_U3WJ4WES

#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/lifetime.hpp>
#include <vanetza/geonet/station_type.hpp>
#include <vanetza/geonet/traffic_class.hpp>
#include <vanetza/units/angle.hpp>
#include <vanetza/units/area.hpp>
#include <vanetza/units/frequency.hpp>
#include <vanetza/units/length.hpp>
#include <vanetza/units/time.hpp>

namespace vanetza
{
namespace geonet
{

enum class UnicastForwarding {
    UNSPECIFIED = 0,
    GREEDY = 1,
    CBF = 2
};

enum class BroadcastForwarding {
    UNSPECIFIED = 0,
    SIMPLE = 1,
    CBF = 2,
    ADVANCED = 3
};

enum class AddrConfMethod {
    AUTO = 0,
    MANAGED = 1,
    ANONYMOUS = 2
};

enum class InterfaceType {
    UNSPECIFIED = 0,
    ITS_G5 = 1
};

enum class SecurityDecapHandling {
    STRICT = 0,
    NON_STRICT = 1
};

struct ManagementInformationBase
{
    ManagementInformationBase();

    Address itsGnLocalGnAddr;
    AddrConfMethod itsGnLocalAddrConfMethod;
    unsigned itsGnProtocolVersion;
    StationType itsGnStationType;
    bool itsGnIsMobile;
    InterfaceType itsGnIfType;
    units::Frequency itsGnMinimumUpdateFrequencyLPV;
    units::Length itsGnPaiInterval;
    unsigned itsGnMaxSduSize;
    unsigned itsGnMaxGeoNetworkingHeaderSize;
    units::Duration itsGnLifetimeLocTE;
    bool itsGnSecurity;
    SecurityDecapHandling itsGnSnDecapResultHandling;
    unsigned itsGnLocationServiceMaxRetrans;
    units::Duration itsGnLocationServiceRetransmitTimer;
    unsigned itsGnLocationServicePacketBufferSize; // byte
    units::Duration itsGnBeaconServiceRetransmitTimer;
    units::Duration itsGnBeaconServiceMaxJitter;
    unsigned itsGnDefaultHopLimit;
    Lifetime itsGnMaxPacketLifetime;
    Lifetime itsGnDefaultPacketLifetime;
    unsigned itsGnMaxPacketDataRate; // kbyte/s
    units::Area itsGnMaxGeoAreaSize;
    units::Duration itsGnMinPacketRepetitionInterval;
    UnicastForwarding itsGnGeoUnicastForwardingAlgorithm;
    BroadcastForwarding itsGnGeoBroadcastForwardingAlgorithm;
    units::Duration itsGnGeoUnicastCbfMinTime;
    units::Duration itsGnGeoUnicastCbfMaxTime;
    units::Duration itsGnGeoBroadcastCbfMinTime;
    units::Duration itsGnGeoBroadcastCbfMaxTime;
    units::Length itsGnDefaultMaxCommunicationRange;
    units::Angle itsGnBroadcastCBFDefSectorAngle;
    units::Angle itsGnUnicastCBFDefSectorAngle;
    bool itsGnGeoAreaLineForwarding;
    unsigned itsGnUcForwardingPacketBufferSize; // kbyte
    unsigned itsGnBcForwardingPacketBufferSize; // kbyte
    unsigned itsGnCbfPacketBufferSize; // kbyte
    TrafficClass itsGnDefaultTrafficClass;
};

// This name is too clumsy to write it out every time
typedef ManagementInformationBase MIB;

} // namespace geonet
} // namespace vanetza

#endif /* MIB_HPP_U3WJ4WES */

