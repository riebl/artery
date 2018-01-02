#ifndef INTERFACE_HPP_KN7EDWOX
#define INTERFACE_HPP_KN7EDWOX

#include <vanetza/geonet/mib.hpp>

namespace vanetza
{
namespace geonet
{
    enum class UpperProtocol { Unknown, BTP_A, BTP_B, IPv6 };
    enum class TransportType { GUC, GAC, GBC, TSB, SHB };
    typedef InterfaceType CommunicationProfile;
} // namespace geonet
} // namespace vanetza

#endif /* INTERFACE_HPP_KN7EDWOX */

