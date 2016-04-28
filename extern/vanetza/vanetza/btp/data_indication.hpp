#ifndef DATA_INDICATION_HPP_5YZM172D
#define DATA_INDICATION_HPP_5YZM172D

#include <vanetza/btp/header.hpp>
#include <vanetza/geonet/address.hpp>
#include <vanetza/geonet/areas.hpp>
#include <vanetza/geonet/data_indication.hpp>
#include <vanetza/geonet/lifetime.hpp>
#include <vanetza/geonet/position_vector.hpp>
#include <vanetza/geonet/traffic_class.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace vanetza
{
namespace btp
{

struct DataIndication
{
    DataIndication();
    DataIndication(const geonet::DataIndication&, const HeaderB&);

    boost::optional<port_type> source_port;
    port_type destination_port;
    boost::optional<decltype(HeaderB::destination_port_info)> destination_port_info;
    decltype(geonet::DataIndication::destination) destination;
    geonet::ShortPositionVector source_position;
    geonet::TrafficClass traffic_class;
    boost::optional<geonet::Lifetime> remaining_packet_lifetime;
};

} // namespace btp
} // namespace vanetza

#endif /* DATA_INDICATION_HPP_5YZM172D */

