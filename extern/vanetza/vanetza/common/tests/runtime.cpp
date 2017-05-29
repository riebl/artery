#include <gtest/gtest.h>
#include <vanetza/common/runtime.hpp>
#include <chrono>
#include <functional>
#include <string>
#include <vector>

using namespace vanetza;
using std::chrono::hours;
using std::chrono::minutes;
using std::chrono::seconds;

TEST(Runtime, default_construction)
{
    Runtime r;
    EXPECT_EQ(std::chrono::milliseconds(0), r.now().time_since_epoch());
}

TEST(Runtime, time_progress_absolute)
{
    Runtime r;

    const Clock::time_point t1 { hours(27) };
    r.trigger(t1);
    EXPECT_EQ(t1, r.now());

    const Clock::time_point t2 { hours(28) };
    r.trigger(t2);
    EXPECT_EQ(t2, r.now());
}

TEST(Runtime, time_progress_relative)
{
    Runtime r;

    r.trigger(hours(3));
    EXPECT_EQ(Clock::time_point { hours(3) }, r.now());

    r.trigger(hours(2));
    EXPECT_EQ(Clock::time_point { hours(5) }, r.now());
}

TEST(Runtime, sorting)
{
    Runtime r;
    r.trigger(hours(3));
    EXPECT_EQ(Clock::time_point::max(), r.next());

    auto cb = [](Clock::time_point) {};
    const auto tp1 = Clock::time_point { hours(2) };
    r.schedule(tp1, cb);
    EXPECT_EQ(tp1, r.next());

    r.schedule(Clock::time_point { hours(3) }, cb);
    EXPECT_EQ(tp1, r.next());

    const auto tp2 = Clock::time_point { hours(1) };
    r.schedule(tp2, cb);
    EXPECT_EQ(tp2, r.next());

    r.schedule(minutes(30), cb);
    EXPECT_EQ(tp2, r.next());
}

TEST(Runtime, scheduling)
{
    Runtime r;
    r.trigger(hours(5));

    namespace ph = std::placeholders;
    std::string seq;
    std::vector<Clock::time_point> deadlines;
    auto cb = [&seq, &deadlines](const char* str, Clock::time_point deadline) {
        deadlines.push_back(deadline);
        seq.append(str);
    };

    r.schedule(hours(10), std::bind<void>(cb, "1", ph::_1));
    r.schedule(hours(11), std::bind<void>(cb, "2", ph::_1));
    r.schedule(hours(11), std::bind<void>(cb, "2", ph::_1));
    r.schedule(hours(5), std::bind<void>(cb, "3", ph::_1));

    r.trigger(hours(4));
    EXPECT_EQ("", seq);

    r.trigger(hours(1));
    EXPECT_EQ("3", seq);

    r.trigger(hours(5));
    EXPECT_EQ("31", seq);

    // schedule expired callback (immediate invocation at next trigger)
    r.schedule(Clock::time_point { hours(2) }, std::bind<void>(cb, "4", ph::_1));
    r.trigger(hours(0));
    EXPECT_EQ("314", seq);

    r.trigger(hours(5));
    EXPECT_EQ("31422", seq);

    r.trigger(Clock::time_point::max());
    EXPECT_EQ("31422", seq);

    const std::vector<Clock::time_point> expected_deadlines = {
        Clock::time_point { hours(10) },
        Clock::time_point { hours(15) },
        Clock::time_point { hours(2) },
        Clock::time_point { hours(16) },
        Clock::time_point { hours(16) },
    };
    EXPECT_EQ(expected_deadlines, deadlines);
}

TEST(Runtime, reset)
{
    Runtime r;
    unsigned calls = 0;
    auto cb = [&calls](Clock::time_point) { ++calls; };

    r.trigger(hours(23));
    for (unsigned i = 10; i < 100; ++i) {
        r.schedule(seconds(i), cb);
    }

    r.trigger(seconds(9));
    EXPECT_EQ(0, calls);
    r.trigger(seconds(2));
    EXPECT_EQ(2, calls);
    EXPECT_EQ(Clock::time_point { hours(23) + seconds(11) }, r.now());

    r.reset(Clock::time_point { hours(10) });
    EXPECT_EQ(Clock::time_point { hours(10) }, r.now());
    EXPECT_EQ(90, calls);
    r.trigger(Clock::duration::max());
    EXPECT_EQ(90, calls);
}

TEST(Runtime, cancel)
{
    Runtime r;
    std::vector<char> calls;
    auto cb = [&calls](char c, Clock::time_point) { calls.push_back(c); };

    namespace ph = std::placeholders;
    r.schedule(minutes(3), std::bind<void>(cb, 'a', ph::_1));
    r.schedule(minutes(4), std::bind<void>(cb, 'b', ph::_1), "foo");
    r.schedule(minutes(5), std::bind<void>(cb, 'c', ph::_1));
    r.schedule(minutes(3), std::bind<void>(cb, 'd', ph::_1), "bar");
    r.schedule(minutes(4), std::bind<void>(cb, 'e', ph::_1));
    r.schedule(minutes(5), std::bind<void>(cb, 'f', ph::_1), "foo");
    r.schedule(minutes(6), std::bind<void>(cb, 'g', ph::_1), "doe");
    r.schedule(minutes(7), std::bind<void>(cb, 'h', ph::_1), "");

    // cancel single callback
    r.cancel("bar");
    r.trigger(minutes(8));
    EXPECT_EQ((std::vector<char> {'a', 'b', 'e', 'c', 'f', 'g', 'h'}), calls);

    // cancel several callbacks
    calls.clear();
    r.schedule(minutes(1), std::bind<void>(cb, 'a', ph::_1), "foo");
    r.schedule(minutes(1), std::bind<void>(cb, 'b', ph::_1), "bar");
    r.schedule(minutes(1), std::bind<void>(cb, 'c', ph::_1), "bar");
    r.schedule(minutes(1), std::bind<void>(cb, 'd', ph::_1), "foo");
    r.cancel("foo");
    r.trigger(minutes(1));
    EXPECT_EQ((std::vector<char> {'b', 'c'}), calls);
}
