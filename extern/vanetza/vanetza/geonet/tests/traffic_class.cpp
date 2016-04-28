#include <gtest/gtest.h>
#include <vanetza/geonet/traffic_class.hpp>

using namespace vanetza::geonet;

TEST(TrafficClass, ctor) {
    TrafficClass a;
    EXPECT_FALSE(a.store_carry_forward());
    EXPECT_FALSE(a.channel_offload());
    EXPECT_EQ(a.tc_id(), 0);

    TrafficClass b(true, false, 8);
    EXPECT_TRUE(b.store_carry_forward());
    EXPECT_FALSE(b.channel_offload());
    EXPECT_EQ(b.tc_id(), 8);

    TrafficClass c(false, true, 5);
    EXPECT_FALSE(c.store_carry_forward());
    EXPECT_TRUE(c.channel_offload());
    EXPECT_EQ(c.tc_id(), 5);

    TrafficClass d(true, true, 1);
    EXPECT_TRUE(d.store_carry_forward());
    EXPECT_TRUE(d.channel_offload());
    EXPECT_EQ(d.tc_id(), 1);
}

TEST(TrafficClass, store_carry_forward) {
    TrafficClass a(true, false, 0);
    EXPECT_TRUE(a.store_carry_forward());
    a.store_carry_forward(false);
    EXPECT_FALSE(a.store_carry_forward());
    a.store_carry_forward(true);
    EXPECT_TRUE(a.store_carry_forward());
}

TEST(TrafficClass, channel_offload) {
    TrafficClass a(false, true, 0);
    EXPECT_TRUE(a.channel_offload());
    a.channel_offload(false);
    EXPECT_FALSE(a.channel_offload());
    a.channel_offload(true);
    EXPECT_TRUE(a.channel_offload());
}

TEST(TrafficClass, tc_id) {
    TrafficClass a(false, false, 0);
    EXPECT_EQ(a.tc_id(), 0);
    a.tc_id(7);
    EXPECT_EQ(a.tc_id(), 7);
    a.tc_id(63);
    EXPECT_EQ(a.tc_id(), 63);
    a.tc_id(0);
    EXPECT_EQ(a.tc_id(), 0);
}

TEST(TrafficClass, map_tc_onto_profile) {
    using vanetza::dcc::Profile;
    TrafficClass tc;

    tc.tc_id(0);
    EXPECT_EQ(Profile::DP0, map_tc_onto_profile(tc));

    tc.tc_id(1);
    EXPECT_EQ(Profile::DP1, map_tc_onto_profile(tc));

    tc.tc_id(2);
    EXPECT_EQ(Profile::DP2, map_tc_onto_profile(tc));

    tc.tc_id(3);
    EXPECT_EQ(Profile::DP3, map_tc_onto_profile(tc));

    tc.tc_id(4);
    EXPECT_EQ(Profile::DP3, map_tc_onto_profile(tc));
}
