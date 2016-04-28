#include "runtime.hpp"
#include <cassert>

namespace vanetza
{

Runtime::Runtime(Clock::time_point init) : m_now(init)
{
}

void Runtime::schedule(Clock::time_point tp, const Callback& cb, const std::string& name)
{
    m_queue.insert(queue_bimap::value_type { tp, name, cb });
}

void Runtime::schedule(Clock::duration d, const Callback& cb, const std::string& name)
{
    schedule(m_now + d, cb, name);
}

void Runtime::cancel(const std::string& name)
{
    auto name_match_range = m_queue.right.equal_range(name);
    m_queue.right.erase(name_match_range.first, name_match_range.second);
}

Clock::time_point Runtime::next() const
{
    Clock::time_point next_tp = Clock::time_point::max();
    if (!m_queue.empty()) {
        next_tp = m_queue.left.begin()->first;
    }
    return next_tp;
}

const Clock::time_point& Runtime::now() const
{
    return m_now;
}

void Runtime::trigger(Clock::time_point tp)
{
    // require monotonic clock
    assert(tp >= m_now);
    m_now = tp;
    trigger();
}

void Runtime::trigger(Clock::duration d)
{
    m_now += d;
    trigger();
}

void Runtime::trigger()
{
    // process queue elements separately because callback might modify runtime
    while (!m_queue.empty()) {
        auto top = m_queue.left.begin();
        const auto deadline = top->first;
        if (deadline <= m_now) {
            Callback cb = top->info;
            m_queue.left.erase(top);
            // callback invocation has to be last action because it might modify runtime
            cb(deadline);
        } else {
            break;
        }
    }
}

void Runtime::reset(Clock::time_point tp)
{
    m_now = tp;
    queue_bimap queue;
    swap(queue, m_queue);

    // invoke all callbacks once so they can re-schedule
    for (auto& item : queue) {
        const auto& deadline = item.left;
        auto& callback = item.info;
        // callback might modify m_queue
        callback(deadline);
    }
}

} // namespace vanetza
