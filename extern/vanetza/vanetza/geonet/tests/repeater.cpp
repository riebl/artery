#include <gtest/gtest.h>
#include <vanetza/common/runtime.hpp>
#include <vanetza/geonet/data_request.hpp>
#include <vanetza/geonet/mib.hpp>
#include <vanetza/geonet/packet.hpp>
#include <vanetza/geonet/repeater.hpp>

using namespace vanetza;
using namespace vanetza::geonet;

struct FakeRepetitionDispatcher : public boost::static_visitor<>
{
    FakeRepetitionDispatcher(Repeater& _repeater, const DownPacket& _packet) :
        repeater(_repeater), packet(std::move(_packet))
    {
    }

    template<typename REQUEST>
    void operator()(const REQUEST& request)
    {
        repeater.add(request, std::move(packet));
    }

    Repeater& repeater;
    const DownPacket& packet;
};

class RepeaterTest : public ::testing::Test
{
protected:
    RepeaterTest() : repeater(runtime, repetition_callback()), dispatch_counter(0)
    {
    }

    void SetUp() override
    {
        runtime.trigger(std::chrono::hours(23));
        dispatch_counter = 0;
    }

    void dispatch_repetition(const DataRequestVariant& request, std::unique_ptr<DownPacket> packet)
    {
        ++dispatch_counter;
        FakeRepetitionDispatcher dispatcher(repeater, *packet);
        boost::apply_visitor(dispatcher, request);
    }

    Repeater::Callback repetition_callback()
    {
        namespace ph = std::placeholders;
        return std::bind(&RepeaterTest::dispatch_repetition, this, ph::_1, ph::_2);
    }

    MIB mib;
    Runtime runtime;
    Repeater repeater;
    unsigned dispatch_counter;
    const DownPacket packet;
};



TEST_F(RepeaterTest, no_repetition) {
    runtime.trigger(std::chrono::seconds(3));
    EXPECT_EQ(0, dispatch_counter);

    ShbDataRequest shb(mib);
    EXPECT_FALSE(!!shb.repetition);
    repeater.add(shb, packet);
    runtime.trigger(std::chrono::seconds(3));
    EXPECT_EQ(0, dispatch_counter);

    DataRequest::Repetition repetition;
    repetition.interval = 5.0 * units::si::seconds;
    repetition.maximum = 4.9 * units::si::seconds;
    shb.repetition = repetition;
    repeater.add(shb, packet);
    EXPECT_EQ(0, dispatch_counter);
    runtime.trigger(std::chrono::seconds(10));
    EXPECT_EQ(0, dispatch_counter);
}

TEST_F(RepeaterTest, single_repetition) {
    ShbDataRequest shb(mib);
    DataRequest::Repetition repetition;
    repetition.interval = 1.0 * units::si::seconds;
    repetition.maximum = 1.0 * units::si::seconds;
    shb.repetition = repetition;
    repeater.add(shb, packet);
    EXPECT_EQ(0, dispatch_counter);
    runtime.trigger(runtime.now());
    EXPECT_EQ(0, dispatch_counter);
    runtime.trigger(std::chrono::milliseconds(900));
    EXPECT_EQ(0, dispatch_counter);
    runtime.trigger(std::chrono::seconds(1));
    EXPECT_EQ(1, dispatch_counter);
    runtime.trigger(std::chrono::seconds(5));
    EXPECT_EQ(1, dispatch_counter);
}

TEST_F(RepeaterTest, multiple_repetition) {
    ShbDataRequest shb(mib);
    DataRequest::Repetition repetition;
    repetition.interval = 2.0 * units::si::seconds;
    repetition.maximum = 9.0 * units::si::seconds;
    shb.repetition = repetition;
    repeater.add(shb, packet);
    EXPECT_EQ(0, dispatch_counter);
    runtime.trigger(runtime.now());
    EXPECT_EQ(0, dispatch_counter);
    runtime.trigger(std::chrono::milliseconds(1900));
    EXPECT_EQ(0, dispatch_counter);
    runtime.trigger(std::chrono::milliseconds(100));
    EXPECT_EQ(1, dispatch_counter);
    runtime.trigger(std::chrono::seconds(2));
    EXPECT_EQ(2, dispatch_counter);
    // When triggered too slowly (large time difference), still just one repetition is triggered
    runtime.trigger(std::chrono::seconds(5));
    EXPECT_EQ(3, dispatch_counter);
    runtime.trigger(std::chrono::seconds(10));
    EXPECT_EQ(3, dispatch_counter);
}

