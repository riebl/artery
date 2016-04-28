#ifndef DATA_INDICATION_HPP_DOJK9Q8T
#define DATA_INDICATION_HPP_DOJK9Q8T

#include <vanetza/geonet/destination_variant.hpp>
#include <vanetza/geonet/interface.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/security/decap_confirm.hpp>
#include <boost/optional.hpp>

namespace vanetza
{
namespace geonet
{

// forward declarations
struct BasicHeader;
struct CommonHeader;

struct DataIndication
{
    DataIndication();
    DataIndication(const BasicHeader&, const CommonHeader&);

    UpperProtocol upper_protocol;
    TransportType transport_type;
    DestinationVariant destination;
    ShortPositionVector source_position;
    security::ReportType security_report;
    // TODO: certificate id and permissions are missing (optional)
    TrafficClass traffic_class;
    boost::optional<Lifetime> remaining_packet_lifetime;
    boost::optional<unsigned> remaining_hop_limit;
};

} // namespace geonet
} // namespace vanetza

#endif /* DATA_INDICATION_HPP_DOJK9Q8T */

