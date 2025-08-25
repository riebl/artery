#include "pybind11/detail/common.h"
#include "pybind11/pybind11.h"
#include "pybind11/pytypes.h"
#include "vanetza/btp/data_request.hpp"
#include "vanetza/common/its_aid.hpp"
#include "vanetza/geonet/interface.hpp"

#include <pybind11/embed.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <vanetza/net/packet.hpp>

namespace py = pybind11;

/*
 * Modules that do not depend on embedded omnet/artery functionality
 * should be exported as shared libraries that can be used
 * outside of runtime for proper testing.
 */

PYBIND11_MODULE(_vanetza_btp, m)
{
    py::enum_<vanetza::geonet::TransportType>(m, "TransportType")
        .value("GAC", vanetza::geonet::TransportType::GAC)
        .value("GUC", vanetza::geonet::TransportType::GUC)
        .value("GBC", vanetza::geonet::TransportType::GBC)
        .value("TSB", vanetza::geonet::TransportType::TSB)
        .value("SHB", vanetza::geonet::TransportType::SHB);

    py::enum_<vanetza::geonet::CommunicationProfile>(m, "CommunicationProfile")
        .value("Unspecified", vanetza::geonet::CommunicationProfile::Unspecified)
        .value("ITS_G5", vanetza::geonet::CommunicationProfile::ITS_G5)
        .value("LTE_V2X", vanetza::geonet::CommunicationProfile::LTE_V2X);

    py::class_<vanetza::btp::DataRequestGeoNetParams>(m, "DataRequestGeoNetParams")
        .def_readwrite("transport_type", &vanetza::btp::DataRequestGeoNetParams::transport_type);

    py::class_<vanetza::btp::DataRequestB>(m, "DataRequestB")
        .def_readwrite("destination_port", &vanetza::btp::DataRequestB::destination_port)
        .def_readwrite("destination_port_info", &vanetza::btp::DataRequestB::destination_port_info);

    // struct DataRequestGeoNetParams
    // {
    //     geonet::TransportType transport_type;
    //     geonet::DestinationVariant destination;
    //     geonet::CommunicationProfile communication_profile;
    //     ItsAid its_aid;
    //     boost::optional<geonet::Lifetime> maximum_lifetime;
    //     boost::optional<unsigned> maximum_hop_limit;
    //     boost::optional<geonet::DataRequest::Repetition> repetition;
    //     geonet::TrafficClass traffic_class;
    // };
}