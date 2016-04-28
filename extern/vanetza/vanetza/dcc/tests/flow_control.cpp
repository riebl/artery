#include <gtest/gtest.h>
#include <vanetza/access/data_request.hpp>
#include <vanetza/access/interface.hpp>
#include <vanetza/common/runtime.hpp>
#include <vanetza/dcc/flow_control.hpp>
#include <vanetza/dcc/scheduler.hpp>
#include <vanetza/dcc/state_machine.hpp>
#include <chrono>

using namespace vanetza;
using namespace vanetza::dcc;
using namespace std::chrono;

class FakeAccessInterface : public access::Interface
{
public:
    void request(const access::DataRequest& req, std::unique_ptr<ChunkPacket> packet) override
    {
        last_request = req;
        last_packet = std::move(packet);
        ++transmissions;
    }

    boost::optional<access::DataRequest> last_request;
    std::unique_ptr<ChunkPacket> last_packet;
    unsigned transmissions = 0;
};

class FlowControlTest : public testing::Test
{
protected:
    FlowControlTest() :
        runtime(), scheduler(fsm, runtime.now()),
        flow_control(runtime, scheduler, access)
    {}

    std::unique_ptr<ChunkPacket> create_packet(std::size_t length = 0)
    {
        std::unique_ptr<ChunkPacket> packet { new ChunkPacket() };
        packet->layer(OsiLayer::Application) = ByteBuffer(length);
        return packet;
    }

    MacAddress mac(char x)
    {
        return MacAddress { 0, 0, 0, 0, 0, static_cast<uint8_t>(x) };
    }

    Runtime runtime;
    StateMachine fsm;
    Scheduler scheduler;
    FakeAccessInterface access;
    FlowControl flow_control;
};

TEST_F(FlowControlTest, immediate_transmission)
{
    EXPECT_EQ(Clock::duration::zero(), scheduler.delay(Profile::DP1));
    EXPECT_FALSE(access.last_request);
    DataRequest request;
    request.dcc_profile = Profile::DP1;
    flow_control.request(request, create_packet());
    ASSERT_TRUE(!!access.last_request);
    EXPECT_EQ(AccessCategory::VI, access.last_request->access_category);

    EXPECT_LT(Clock::duration::zero(), scheduler.delay(Profile::DP2));
    request.dcc_profile = Profile::DP2;
    access.last_request = boost::none;
    flow_control.request(request, create_packet());
    EXPECT_FALSE(access.last_request);

    EXPECT_EQ(Clock::duration::zero(), scheduler.delay(Profile::DP0));
    request.dcc_profile = Profile::DP0;
    flow_control.request(request, create_packet());
    ASSERT_TRUE(!!access.last_request);
    EXPECT_EQ(AccessCategory::VO, access.last_request->access_category);
}

TEST_F(FlowControlTest, queuing)
{
    DataRequest request;
    request.lifetime = hours(1); // expired lifetime shall be no concern here

    scheduler.notify(Profile::DP1);
    EXPECT_LT(Clock::duration::zero(), scheduler.delay(Profile::DP1));
    EXPECT_LT(Clock::duration::zero(), scheduler.delay(Profile::DP2));
    EXPECT_LT(Clock::duration::zero(), scheduler.delay(Profile::DP3));

    request.destination = mac(1);
    request.dcc_profile = Profile::DP1;
    flow_control.request(request, create_packet());
    request.destination = mac(2);
    request.dcc_profile = Profile::DP3;
    flow_control.request(request, create_packet());
    request.destination = mac(3);
    request.dcc_profile = Profile::DP2;
    flow_control.request(request, create_packet());

    runtime.trigger(scheduler.delay(Profile::DP1));
    ASSERT_TRUE(!!access.last_request);
    EXPECT_EQ(mac(1), access.last_request->destination_addr);
    EXPECT_EQ(1, access.transmissions);

    runtime.trigger(scheduler.delay(Profile::DP2) / 2);
    EXPECT_EQ(1, access.transmissions);

    runtime.trigger(scheduler.delay(Profile::DP2));
    EXPECT_EQ(2, access.transmissions);
    EXPECT_EQ(mac(3), access.last_request->destination_addr);

    request.destination = mac(4);
    request.dcc_profile = Profile::DP2;
    flow_control.request(request, create_packet());
    request.destination = mac(5);
    request.dcc_profile = Profile::DP3;
    flow_control.request(request, create_packet());

    runtime.trigger(scheduler.delay(Profile::DP2));
    EXPECT_EQ(3, access.transmissions);
    EXPECT_EQ(mac(4), access.last_request->destination_addr);

    runtime.trigger(scheduler.delay(Profile::DP3));
    EXPECT_EQ(4, access.transmissions);
    EXPECT_EQ(mac(2), access.last_request->destination_addr);

    runtime.trigger(scheduler.delay(Profile::DP3));
    EXPECT_EQ(5, access.transmissions);
    EXPECT_EQ(mac(5), access.last_request->destination_addr);

    // no future transmissions queued anymore
    runtime.trigger(Clock::time_point::max());
    EXPECT_EQ(5, access.transmissions);
}

TEST_F(FlowControlTest, drop_expired)
{
    std::list<AccessCategory> drops;
    flow_control.set_packet_drop_hook([&drops](AccessCategory ac) {
            drops.push_back(ac);
    });

    scheduler.notify(Profile::DP3);
    DataRequest request;
    request.dcc_profile = Profile::DP3;
    request.lifetime = scheduler.delay(Profile::DP3) - milliseconds(10);
    flow_control.request(request, create_packet());
    runtime.trigger(scheduler.delay(Profile::DP3) + milliseconds(10));
    EXPECT_FALSE(access.last_request);
    ASSERT_FALSE(drops.empty());
    EXPECT_EQ(AccessCategory::BK, drops.back());
    EXPECT_EQ(0, access.transmissions);

    scheduler.notify(Profile::DP3);
    auto delay = scheduler.delay(Profile::DP3);
    EXPECT_NE(Clock::duration::zero(), delay);
    request.lifetime = delay;
    flow_control.request(request, create_packet());
    request.lifetime = delay / 2;
    flow_control.request(request, create_packet());
    request.lifetime = 3 * delay / 2;
    flow_control.request(request, create_packet());
    request.lifetime = 2 * delay;
    flow_control.request(request, create_packet());
    request.lifetime = delay / 4;
    flow_control.request(request, create_packet());
    runtime.trigger(delay);
    EXPECT_EQ(3, drops.size());
    EXPECT_EQ(1, access.transmissions);
    runtime.trigger(delay);
    EXPECT_EQ(4, drops.size());
    EXPECT_EQ(2, access.transmissions);

    // all queues should be empty now, no future transmissions
    runtime.trigger(Clock::time_point::max());
    EXPECT_EQ(2, access.transmissions);
}

TEST_F(FlowControlTest, queue_length)
{
    // set queue length limit (default is unlimited)
    flow_control.queue_length(2);

    // count drops
    std::size_t drops = 0;
    flow_control.set_packet_drop_hook([&drops](AccessCategory) { ++drops; });

    DataRequest request;
    request.dcc_profile = Profile::DP1;
    request.lifetime = std::chrono::seconds(5);

    // cause enqueuing of arriving DP1 packets
    scheduler.notify(Profile::DP1);
    ASSERT_LT(Clock::duration::zero(), scheduler.delay(Profile::DP1));

    flow_control.request(request, create_packet(1));
    flow_control.request(request, create_packet(2));
    EXPECT_EQ(0, access.transmissions);
    EXPECT_EQ(0, drops);

    flow_control.request(request, create_packet(3));
    EXPECT_EQ(0, access.transmissions);
    EXPECT_EQ(1, drops);

    runtime.trigger(scheduler.delay(Profile::DP1));
    EXPECT_EQ(1, access.transmissions);
    EXPECT_EQ(2, access.last_packet->size());

    runtime.trigger(scheduler.delay(Profile::DP1));
    EXPECT_EQ(2, access.transmissions);
    EXPECT_EQ(3, access.last_packet->size());
    EXPECT_EQ(1, drops);
}
