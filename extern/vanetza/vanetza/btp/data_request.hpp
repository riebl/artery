#ifndef DATA_REQUEST_HPP_BSJC1VFV
#define DATA_REQUEST_HPP_BSJC1VFV

#include <vanetza/btp/header.hpp>
#include <vanetza/geonet/data_request.hpp>
#include <vanetza/geonet/destination_variant.hpp>
#include <vanetza/geonet/interface.hpp>
#include <vanetza/geonet/lifetime.hpp>
#include <vanetza/geonet/traffic_class.hpp>
#include <vanetza/security/profile.hpp>

namespace vanetza
{
namespace btp
{

struct DataRequestGeoNetParams
{
    geonet::TransportType transport_type;
    geonet::DestinationVariant destination;
    geonet::CommunicationProfile communication_profile;
    security::Profile security_profile;
    boost::optional<geonet::Lifetime> maximum_lifetime;
    boost::optional<geonet::DataRequest::Repetition> repetition;
    geonet::TrafficClass traffic_class;
};

struct DataRequestB
{
    DataRequestB();

    decltype(HeaderB::destination_port) destination_port;
    decltype(HeaderB::destination_port_info) destination_port_info;
    DataRequestGeoNetParams gn;
};

} // namespace btp
} // namespace vanetza

#endif /* DATA_REQUEST_HPP_BSJC1VFV */

