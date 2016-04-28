#include <gtest/gtest.h>
#include <vanetza/btp/data_indication.hpp>
#include <vanetza/btp/header.hpp>
#include <vanetza/geonet/data_indication.hpp>
#include <boost/optional/optional_io.hpp>

using namespace vanetza;

TEST(BtpDataIndication, construct_from_header_b) {
    geonet::DataIndication gn_ind;
    gn_ind.transport_type = geonet::TransportType::GUC;
    gn_ind.destination = geonet::Address { MacAddress { 1, 2, 3, 4, 5, 6 }};
    gn_ind.traffic_class.store_carry_forward(true);
    gn_ind.source_position.longitude =
        static_cast<geonet::geo_angle_i32t>(43.8 * units::degree);
    gn_ind.remaining_packet_lifetime =
        geonet::Lifetime(geonet::Lifetime::Base::_10_S, 3);

    btp::HeaderB hdr;
    hdr.destination_port = host_cast<uint16_t>(0x1816);
    hdr.destination_port_info = host_cast<uint16_t>(0x4711);;

    btp::DataIndication btp_ind(gn_ind, hdr);
    EXPECT_FALSE(btp_ind.source_port);
    EXPECT_EQ(hdr.destination_port, btp_ind.destination_port);
    EXPECT_EQ(hdr.destination_port_info, btp_ind.destination_port_info);
    EXPECT_EQ(boost::get<geonet::Address>(gn_ind.destination),
            boost::get<geonet::Address>(btp_ind.destination));
    EXPECT_EQ(gn_ind.source_position, btp_ind.source_position);
    EXPECT_EQ(gn_ind.traffic_class.raw(), btp_ind.traffic_class.raw());
    ASSERT_TRUE(!!btp_ind.remaining_packet_lifetime);
    EXPECT_EQ(gn_ind.remaining_packet_lifetime->raw(),
            btp_ind.remaining_packet_lifetime->raw());
}

