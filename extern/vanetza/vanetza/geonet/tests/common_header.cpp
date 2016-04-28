#include <gtest/gtest.h>
#include <vanetza/geonet/common_header.hpp>
#include <vanetza/geonet/data_request.hpp>

using namespace vanetza::geonet;

TEST(CommonHeader, ctor) {
    MIB mib;
    CommonHeader a(mib);
    EXPECT_EQ(a.traffic_class.raw(), mib.itsGnDefaultTrafficClass.raw());
    EXPECT_EQ(a.maximum_hop_limit, mib.itsGnDefaultHopLimit);
    EXPECT_EQ(a.payload, 0);

    DataRequest req(mib);
    req.upper_protocol = UpperProtocol::BTP_B;
    req.max_hop_limit = 3;
    req.traffic_class.store_carry_forward(true);
    CommonHeader b(req, mib);
    EXPECT_EQ(b.next_header, NextHeaderCommon::BTP_B);
    EXPECT_EQ(b.maximum_hop_limit, 3);
    EXPECT_TRUE(b.traffic_class.store_carry_forward());

    ShbDataRequest shb(mib);
    CommonHeader c(shb, mib);
    EXPECT_EQ(c.header_type, HeaderType::TSB_SINGLE_HOP);
    EXPECT_EQ(c.maximum_hop_limit, 1);
}

