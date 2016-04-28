#include <gtest/gtest.h>
#include <vanetza/common/clock.hpp>
#include <vanetza/dcc/scheduler.hpp>
#include <vanetza/dcc/state_machine.hpp>

using namespace std::chrono;
using namespace vanetza::dcc;
using vanetza::Clock;

static const Clock::duration immediately = milliseconds(0);

class SchedulerTest : public ::testing::Test
{
protected:
    SchedulerTest() : now(seconds(4711)), sc(fsm, now) {}

    Clock::time_point now;
    StateMachine fsm;
    Scheduler sc;
};

TEST_F(SchedulerTest, burst)
{
    for (unsigned i = 0; i < 20; ++i) {
        now += milliseconds(49);
        EXPECT_EQ(immediately, sc.delay(Profile::DP0));
        sc.notify(Profile::DP0);
    }

    now += milliseconds(20);
    EXPECT_GT(seconds(10), sc.delay(Profile::DP0));
    EXPECT_LT(seconds(9), sc.delay(Profile::DP0));
}

TEST_F(SchedulerTest, regular)
{
    const auto tx_int = milliseconds(60);
    ASSERT_EQ(tx_int, fsm.transmission_interval());

    EXPECT_EQ(immediately, sc.delay(Profile::DP1));
    sc.notify(Profile::DP1);
    EXPECT_EQ(tx_int, sc.delay(Profile::DP1));

    now += milliseconds(50);
    EXPECT_EQ(milliseconds(10), sc.delay(Profile::DP1));
    EXPECT_EQ(milliseconds(10), sc.delay(Profile::DP2));
    EXPECT_EQ(milliseconds(10), sc.delay(Profile::DP3));

    now += milliseconds(20);
    EXPECT_EQ(immediately, sc.delay(Profile::DP1));
    EXPECT_EQ(immediately, sc.delay(Profile::DP2));
    EXPECT_EQ(immediately, sc.delay(Profile::DP3));
}

TEST_F(SchedulerTest, burst_regular_independence)
{
    ASSERT_EQ(immediately, sc.delay(Profile::DP1));

    // consume whole burst budget
    for (unsigned i = 0; i < 20; ++i) {
        sc.notify(Profile::DP0);
    }
    ASSERT_LT(immediately, sc.delay(Profile::DP0));

    // can send regular budget messages nonetheless
    EXPECT_EQ(immediately, sc.delay(Profile::DP3));

    // recover burst budget
    now += std::chrono::seconds(20);
    ASSERT_EQ(immediately, sc.delay(Profile::DP0));

    // use regular budget
    EXPECT_EQ(immediately, sc.delay(Profile::DP2));
    sc.notify(Profile::DP2);
    EXPECT_LT(immediately, sc.delay(Profile::DP2));

    // burst budget is not influenced
    EXPECT_EQ(immediately, sc.delay(Profile::DP0));
}

