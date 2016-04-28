#include <gtest/gtest.h>
#include <vanetza/dcc/state_machine.hpp>

using std::chrono::milliseconds;
using namespace vanetza::dcc;


TEST(StateMachine, ctor)
{
    StateMachine sm;
    EXPECT_STREQ("Relaxed", sm.state().name());
    EXPECT_EQ(milliseconds(60), sm.transmission_interval());
    EXPECT_NEAR(16.66, sm.message_rate(), 0.01);
}

TEST(StateMachine, ramp_up)
{
    StateMachine sm;

    // keep below minChannelLoad at first: relaxed
    sm.update(ChannelLoad(30, 100)); // smoothed: 0.15
    sm.update(ChannelLoad(17, 100)); // smoothed: 0.16
    EXPECT_STREQ("Relaxed", sm.state().name());

    // now exceed minChannelLoad for 10 samples: active 1
    sm.update(ChannelLoad(29, 100)); // smoothed: 0.225
    for (unsigned i = 0; i < 8; ++i) {
        sm.update(ChannelLoad(20, 100));
        EXPECT_STREQ("Relaxed", sm.state().name());
    }
    sm.update(ChannelLoad(20, 100)); // smoothed: > 0.20
    EXPECT_STREQ("Active 1", sm.state().name());

    // now let's jump to active 3 directly
    sm.update(ChannelLoad(54, 100)); // smoothed: 0.37
    EXPECT_STREQ("Active 3", sm.state().name());

    // go to restrictive state (over active 5)
    sm.update(ChannelLoad(83, 100)); // smoothed: 0.60
    EXPECT_STREQ("Active 5", sm.state().name());
    for (unsigned i = 0; i < 8; ++i) {
        sm.update(ChannelLoad(60, 100));
        EXPECT_STREQ("Active 5", sm.state().name());
    }
    sm.update(ChannelLoad(60, 100));
    EXPECT_STREQ("Restrictive", sm.state().name());
}

TEST(StateMachine, ramp_down)
{
    StateMachine sm;

    // fill up CL ring buffer first and go to restrictive state
    for (unsigned i = 0; i < 100; ++i) {
        sm.update(ChannelLoad(70, 100));
    }
    ASSERT_STREQ("Restrictive", sm.state().name());

    // cool down to CL = 50%
    // ring buffer: 60.0, 55.0, 52.5, 51.25, 50.625...
    for (unsigned i = 0; i < 50; ++i) {
        sm.update(ChannelLoad(50, 100));
    }
    EXPECT_STREQ("Restrictive", sm.state().name());

    // ring buffer: 55.0, 52.5... -> active 5
    sm.update(ChannelLoad(50, 100));
    EXPECT_STREQ("Active 5", sm.state().name());

    // ring buffer: 51.25, 50.625... -> active 5
    sm.update(ChannelLoad(50, 100));
    sm.update(ChannelLoad(50, 100));
    EXPECT_STREQ("Active 5", sm.state().name());

    // ring buffer: 50.625... -> active 4
    sm.update(ChannelLoad(50, 100));
    EXPECT_STREQ("Active 4", sm.state().name());
}

TEST(State, relaxed)
{
    Relaxed relaxed;
    EXPECT_STREQ("Relaxed", relaxed.name());
    EXPECT_EQ(milliseconds(60), relaxed.transmission_interval());
}

TEST(State, active)
{
    Active active;
    EXPECT_STREQ("Active 1", active.name());
    EXPECT_EQ(milliseconds(100), active.transmission_interval());

    active.update(0.20, 0.36);
    EXPECT_STREQ("Active 3", active.name());
    EXPECT_EQ(milliseconds(260), active.transmission_interval());

    active.update(0.51, 0.52);
    EXPECT_STREQ("Active 5", active.name());
    EXPECT_EQ(milliseconds(420), active.transmission_interval());

    active.update(0.30, 0.44);
    EXPECT_STREQ("Active 4", active.name());
    EXPECT_EQ(milliseconds(340), active.transmission_interval());

    active.update(0.20, 0.30);
    EXPECT_STREQ("Active 2", active.name());
    EXPECT_EQ(milliseconds(180), active.transmission_interval());
}

TEST(State, restrictive)
{
    Restrictive restrictive;
    EXPECT_STREQ("Restrictive", restrictive.name());
    EXPECT_EQ(milliseconds(460), restrictive.transmission_interval());
}

