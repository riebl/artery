#ifndef PDU_VARIANT_HPP_AAEVKD5M
#define PDU_VARIANT_HPP_AAEVKD5M

#include <vanetza/geonet/extended_pdu.hpp>
#include <vanetza/geonet/beacon_header.hpp>
#include <vanetza/geonet/gbc_header.hpp>
#include <vanetza/geonet/shb_header.hpp>
#include <boost/variant.hpp>

namespace vanetza
{
namespace geonet
{

typedef ExtendedPdu<ShbHeader> ShbPdu;
typedef ExtendedPdu<BeaconHeader> BeaconPdu;
typedef ExtendedPdu<GeoBroadcastHeader> GbcPdu;

typedef boost::variant<BeaconPdu, GbcPdu, ShbPdu> PduVariant;

} // namespace geonet
} // namespace vanetza

#endif /* PDU_VARIANT_HPP_AAEVKD5M */

