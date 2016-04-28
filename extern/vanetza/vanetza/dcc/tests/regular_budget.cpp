#include <gtest/gtest.h>
#include <vanetza/common/clock.hpp>
#include <vanetza/dcc/regular_budget.hpp>
#include <vanetza/dcc/state_machine.hpp>

using namespace vanetza::dcc;
using vanetza::Clock;
using std::chrono::milliseconds;

static const Clock::duration immediately = milliseconds(0);

class RegularBudgetTest : public ::testing::Test
{
protected:
    RegularBudgetTest() : now(std::chrono::seconds(4711)), budget(fsm, now) {}

    Clock::time_point now;
    StateMachine fsm;
    RegularBudget budget;
};

TEST_F(RegularBudgetTest, relaxed)
{
    Relaxed relaxed;
    const auto relaxed_tx_interval = relaxed.transmission_interval();
    ASSERT_EQ(relaxed_tx_interval, fsm.transmission_interval());

    EXPECT_EQ(immediately, budget.delay());
    budget.notify();
    EXPECT_EQ(relaxed_tx_interval, budget.delay());

    now += relaxed_tx_interval - milliseconds(10);
    EXPECT_EQ(milliseconds(10), budget.delay());

    now += milliseconds(20);
    EXPECT_EQ(immediately, budget.delay());
}

TEST_F(RegularBudgetTest, restrictive)
{
    Restrictive restrictive;
    const auto restrictive_tx_interval = restrictive.transmission_interval();

    // bring smoothed channel load above 59 %
    fsm.update(ChannelLoad(100, 100)); // 50 %
    fsm.update(ChannelLoad(70, 100)); // 60 %
    for (unsigned i = 0; i < 9; ++i) {
        EXPECT_GT(restrictive_tx_interval, fsm.transmission_interval());
        fsm.update(ChannelLoad(60, 100)); // 60 %
    }

    // by now we should have enough high load samples for restrictive state
    ASSERT_STREQ("Restrictive", fsm.state().name());

    EXPECT_EQ(immediately, budget.delay());
    budget.notify();
    EXPECT_EQ(restrictive_tx_interval, budget.delay());

    now += restrictive_tx_interval / 2;
    EXPECT_EQ(restrictive_tx_interval / 2, budget.delay());
    now += restrictive_tx_interval / 2;
    EXPECT_EQ(immediately, budget.delay());
}
