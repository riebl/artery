#ifndef FLOW_CONTROL_HPP_PG7RKD8V
#define FLOW_CONTROL_HPP_PG7RKD8V

#include <vanetza/common/clock.hpp>
#include <vanetza/common/hook.hpp>
#include <vanetza/dcc/data_request.hpp>
#include <vanetza/dcc/interface.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/net/access_category.hpp>
#include <vanetza/net/chunk_packet.hpp>
#include <boost/optional/optional.hpp>
#include <list>
#include <memory>
#include <tuple>
#include <map>

namespace vanetza
{

// forward declarations
namespace access { class Interface; }
class Runtime;

namespace dcc
{

// forward declarations
class Scheduler;

/**
 * FlowControl is a gatekeeper above access layer.
 *
 * There is a queue for each access category. Packets might be enqueued
 * because of exceeded transmission intervals determined by Scheduler.
 * If a packet's lifetime expires before transmission it will be dropped.
 */
class FlowControl : public RequestInterface
{
public:
    using PacketDropHook = Hook<AccessCategory>;
    using PacketTransmitHook = Hook<AccessCategory>;

    /**
     * Create FlowControl instance
     * \param rt Runtime used for timed actions, e.g. packet expiry
     * \param scheduler Scheduler providing transmission intervals
     * \param access Interface to access layer
     */
    FlowControl(Runtime&, Scheduler&, access::Interface&);

    /**
     * Request packet transmission
     * \param request DCC request parameters
     * \param packet Packet data
     */
    void request(const DataRequest&, std::unique_ptr<ChunkPacket>) override;

    /**
     * Set callback to be invoked at packet drop. Replaces any previous callback.
     * \param cb Callback
     */
    void set_packet_drop_hook(PacketDropHook::callback_type&&);

    /**
     * Set callback to be invoked at packet transmission. Replaces any previous callback.
     * \param cb Callback
     */
    void set_packet_transmit_hook(PacketTransmitHook::callback_type&&);

    /**
     * Set length of each queue
     *
     * The first queue element is dropped when the length limit is hit.
     * \param length Maximum number of queue elements, 0 for unlimited length
     */
    void queue_length(std::size_t length);

private:
    using Transmission = std::tuple<Clock::time_point, DataRequest, std::unique_ptr<ChunkPacket>>;
    using Queue = std::list<Transmission>;

    void enqueue(const DataRequest&, std::unique_ptr<ChunkPacket>);
    boost::optional<Transmission> dequeue();
    void transmit(const DataRequest&, std::unique_ptr<ChunkPacket>);
    bool transmit_immediately(const DataRequest&) const;
    void drop_expired();
    bool empty() const;
    void trigger();
    void schedule_trigger(Profile);
    Transmission* next_transmission();
    Queue* next_queue();

    Runtime& m_runtime;
    Scheduler& m_scheduler;
    access::Interface& m_access;
    std::map<AccessCategory, Queue, std::greater<AccessCategory>> m_queues;
    std::size_t m_queue_length;
    PacketDropHook m_packet_drop_hook;
    PacketTransmitHook m_packet_transmit_hook;
};

} // namespace dcc
} // namespace vanetza

#endif /* FLOW_CONTROL_HPP_PG7RKD8V */

