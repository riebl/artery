#include <gtest/gtest.h>
#include <vanetza/dcc/channel_load_smoothing.hpp>

using namespace vanetza::dcc;

TEST(ChannelLoadSmoothing, update) {
    ChannelLoadSmoothing smoother;
    EXPECT_DOUBLE_EQ(0.0, smoother.channel_load());

    smoother.update(ChannelLoad {50, 100});
    EXPECT_DOUBLE_EQ(0.25, smoother.channel_load());

    smoother.update(ChannelLoad {12500, 12500});
    EXPECT_DOUBLE_EQ(0.625, smoother.channel_load());

    smoother.update(ChannelLoad {0, 100});
    EXPECT_DOUBLE_EQ(0.3125, smoother.channel_load());

    smoother.update(ChannelLoad {0, 1000});
    EXPECT_DOUBLE_EQ(0.15625, smoother.channel_load());
}
