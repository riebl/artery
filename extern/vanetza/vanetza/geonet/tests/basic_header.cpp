#include <gtest/gtest.h>
#include <vanetza/geonet/basic_header.hpp>
#include <vanetza/geonet/data_request.hpp>

using namespace vanetza::geonet;
using vanetza::units::si::seconds;

TEST(BasicHeader, ctor) {
    MIB mib;
    BasicHeader a(mib);
    EXPECT_EQ(a.lifetime, mib.itsGnDefaultPacketLifetime);
    EXPECT_EQ(a.hop_limit, mib.itsGnDefaultHopLimit);

    DataRequest req(mib);
    req.maximum_lifetime.encode(31.0 * seconds);
    req.max_hop_limit = 4;
    BasicHeader b(req, mib);
    EXPECT_EQ(b.lifetime.decode(), 31.0 * seconds);
    EXPECT_EQ(b.hop_limit, 4);

    ShbDataRequest shb(mib);
    BasicHeader c(shb, mib);
    EXPECT_EQ(c.hop_limit, 1);
}
