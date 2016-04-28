#ifndef REPEATER_HPP_AH49FXB1
#define REPEATER_HPP_AH49FXB1

#include <vanetza/common/clock.hpp>
#include <vanetza/geonet/data_request.hpp>
#include <vanetza/geonet/packet.hpp>
#include <functional>
#include <list>
#include <memory>

namespace vanetza
{
// forward declaration
class Runtime;

namespace geonet
{

/**
 * Repeater eases handling of packet repetitions.
 * Packet repetitions are repeated requests with same payload.
 */
class Repeater
{
public:
    using Callback = std::function<void(const DataRequestVariant&, std::unique_ptr<DownPacket>)>;

    /**
     * Create a Repeater instance
     * \param rt runtime used for scheduling repetitions
     * \param cb callback to be invoked when repetition is due
     */
    Repeater(Runtime& rt, const Callback& cb);

    /**
     * Add another repetition
     * \param request any kind of GeoNet data request
     * \param payload request's payload
     */
    template<class REQUEST>
    void add(const REQUEST& request, const DownPacket& payload)
    {
        if (request.repetition) {
            add(request, *request.repetition, payload);
        }
    }

private:
    struct Repetition
    {
        Repetition(const DataRequestVariant&, const DownPacket&);

        DataRequestVariant m_request;
        std::unique_ptr<DownPacket> m_payload;
    };

    /**
     * Add packet repetition to internal book keeping
     */
    void add(const DataRequestVariant&, const DataRequest::Repetition&, const DownPacket&);

    /**
     * Triggered when repetition is due according to runtime
     */
    void trigger(std::list<Repetition>::iterator, Clock::time_point);

    std::list<Repetition> m_repetitions;
    Callback m_repeat_fn;
    Runtime& m_runtime;
};

} // namespace geonet
} // namespace vanetza

#endif /* REPEATER_HPP_AH49FXB1 */

