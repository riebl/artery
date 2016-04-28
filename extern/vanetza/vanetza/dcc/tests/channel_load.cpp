#include <gtest/gtest.h>
#include <vanetza/dcc/channel_load.hpp>

using namespace vanetza::dcc;

TEST(ChannelLoad, ctor)
{
    ChannelLoad cl1;
    EXPECT_EQ(0, cl1.probes_above);
    EXPECT_EQ(0, cl1.probes_total);

    ChannelLoad cl2(30, 250);
    EXPECT_EQ(30, cl2.probes_above);
    EXPECT_EQ(250, cl2.probes_total);
}

 TEST(ChannelLoad, less)
{
    EXPECT_LT(ChannelLoad(30, 100), ChannelLoad(31, 100));
    EXPECT_LT(ChannelLoad(30, 100), ChannelLoad(8, 25));
    EXPECT_LT(ChannelLoad(0,10), ChannelLoad(1, 2));
}
