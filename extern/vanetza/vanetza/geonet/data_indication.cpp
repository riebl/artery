#include "basic_header.hpp"
#include "common_header.hpp"
#include "data_indication.hpp"
#include <stdexcept>

namespace vanetza
{
namespace geonet
{

DataIndication::DataIndication() :
    upper_protocol(UpperProtocol::BTP_B),
    transport_type(TransportType::SHB),
    security_report(security::ReportType::Unsigned_Message)
{
}

DataIndication::DataIndication(const BasicHeader& basic, const CommonHeader& common) :
    transport_type(TransportType::SHB),
    security_report(basic.next_header == NextHeaderBasic::SECURED ?
            security::ReportType::Incompatible_Protocol :
            security::ReportType::Unsigned_Message),
    traffic_class(common.traffic_class),
    remaining_packet_lifetime(basic.lifetime),
    remaining_hop_limit(basic.hop_limit)
{
    switch (common.next_header) {
        case NextHeaderCommon::BTP_A:
            upper_protocol = UpperProtocol::BTP_A;
            break;
        case NextHeaderCommon::BTP_B:
            upper_protocol = UpperProtocol::BTP_B;
            break;
        case NextHeaderCommon::IPv6:
            upper_protocol = UpperProtocol::IPv6;
            break;
        default:
            throw std::runtime_error("No mapping onto upper protocol possible");
            break;
    }
}

} // namespace geonet
} // namespace vanetza
