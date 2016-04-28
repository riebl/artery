#include "repeater.hpp"
#include "data_request.hpp"
#include <vanetza/common/runtime.hpp>
#include <cassert>

namespace vanetza
{
namespace geonet
{

Repeater::Repetition::Repetition(const DataRequestVariant& request, const DownPacket& payload) :
    m_request(request), m_payload(new DownPacket(payload))
{
}

Repeater::Repeater(Runtime& rt, const Callback& cb) :  m_repeat_fn(cb), m_runtime(rt)
{
    assert(m_repeat_fn);
}

void Repeater::add(const DataRequestVariant& request,
        const DataRequest::Repetition& repetition, const DownPacket& payload)
{
    if (has_further_repetition(repetition)) {
        const auto next_repetition = m_runtime.now() + units::clock_cast(repetition.interval);
        m_repetitions.emplace_front(request, payload);
        auto added = m_repetitions.begin();
        auto then = std::placeholders::_1;
        m_runtime.schedule(next_repetition, std::bind<void>(&Repeater::trigger, this, added, then));
    }
}

void Repeater::trigger(std::list<Repetition>::iterator rep, Clock::time_point invocation)
{
    Repetition& repetition = *rep;
    DataRequest& request = access_request(repetition.m_request);
    assert(request.repetition);

    // reduce remaining interval by one step and occurred triggering delay
    decrement_by_one(*request.repetition);
    auto delayed = m_runtime.now() - invocation;
    request.repetition->maximum -= units::clock_cast(delayed);

    // reset repetition data if this is the last repetition
    if (!has_further_repetition(request)) {
        request.repetition.reset();
    }

    m_repeat_fn(repetition.m_request, std::move(repetition.m_payload));
    m_repetitions.erase(rep);
}

} // namespace geonet
} // namespace vanetza

