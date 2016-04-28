#include "channel_load.hpp"
#include "state_machine.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <limits>

namespace vanetza
{
namespace dcc
{

static const std::size_t N_samples_up = std::chrono::seconds(1) / NDL_minDccSampling;
static const std::size_t N_samples_down = std::chrono::seconds(5) / NDL_minDccSampling;

static const double NDL_minChannelLoad = 0.19;
static const double NDL_maxChannelLoad = 0.59;

Clock::duration Relaxed::transmission_interval() const
{
    return std::chrono::milliseconds(60);
}

const char* Relaxed::name() const
{
    return "Relaxed";
}

Clock::duration Restrictive::transmission_interval() const
{
    return std::chrono::milliseconds(460);
}

const char* Restrictive::name() const
{
    return "Restrictive";
}

const std::size_t Active::sc_substates = 5;

Active::Active() : m_substate(0)
{
}

void Active::update(double min_cl, double max_cl)
{
    assert(min_cl <= max_cl);

    static const std::array<double, sc_substates> channel_loads {{
        0.27, 0.35, 0.43, 0.51, 0.59
    }};

    auto state_up_it = std::upper_bound(channel_loads.begin(), channel_loads.end(), min_cl);
    auto state_up = std::distance(channel_loads.begin(), state_up_it);

    auto state_down_it = std::upper_bound(channel_loads.begin(), channel_loads.end(), max_cl);
    auto state_down = std::distance(channel_loads.begin(), state_down_it);

    m_substate = std::max(state_up, state_down);
    m_substate = std::min(sc_substates - 1, m_substate);
    assert(m_substate < sc_substates);
}

Clock::duration Active::transmission_interval() const
{
    static const std::array<Clock::duration, sc_substates> tx_intervals {{
        std::chrono::milliseconds(100),
        std::chrono::milliseconds(180),
        std::chrono::milliseconds(260),
        std::chrono::milliseconds(340),
        std::chrono::milliseconds(420),
    }};

    const std::size_t index = std::min(tx_intervals.size() - 1, m_substate);
    return tx_intervals[index];
}

const char* Active::name() const
{
    static const std::array<const char*, sc_substates> names {{
        "Active 1",
        "Active 2",
        "Active 3",
        "Active 4",
        "Active 5"
    }};

    assert(m_substate < sc_substates);
    return names[m_substate];
}

StateMachine::StateMachine() :
    m_state(&m_relaxed),
    m_channel_loads(std::max(N_samples_up, N_samples_down))
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::update(const ChannelLoad& cl)
{
    m_current_cl = cl;
    m_cl_smoothing.update(cl);
    m_channel_loads.push_front(m_cl_smoothing.channel_load());

    if (m_state == &m_relaxed) {
        if (min_channel_load() >= NDL_minChannelLoad) {
            m_state = &m_active;
            m_active.update(min_channel_load(), max_channel_load());
        }
    } else if (m_state == &m_restrictive) {
        if (max_channel_load() < NDL_maxChannelLoad) {
            m_state = &m_active;
            m_active.update(min_channel_load(), max_channel_load());
        }
    } else {
        if (max_channel_load() < NDL_minChannelLoad) {
            m_state = &m_relaxed;
        } else if (min_channel_load() >= NDL_maxChannelLoad) {
            m_state = &m_restrictive;
        } else {
            m_state = &m_active;
            m_active.update(min_channel_load(), max_channel_load());
        }
    }
}

boost::optional<double> StateMachine::getChannelLoad() const
{
    if (m_current_cl) {
        return m_current_cl->fraction();
    } else {
        return boost::optional<double>();
    }
}

boost::optional<double> StateMachine::getSmoothedChannelLoad() const
{
    if (!m_channel_loads.empty()) {
        return m_channel_loads.front();
    } else {
        return boost::optional<double>();
    }
}

double StateMachine::message_rate() const
{
    std::chrono::duration<double> one_sec = std::chrono::seconds(1);
    return one_sec / transmission_interval();
}

Clock::duration StateMachine::transmission_interval() const
{
    return m_state->transmission_interval();
}

const State& StateMachine::state() const
{
    assert(m_state != nullptr);
    return *m_state;
}

double StateMachine::min_channel_load() const
{
    assert(N_samples_up > 0);
    double min_cl = std::numeric_limits<double>::infinity();
    std::size_t sample_cnt = 0;
    for (auto sample : m_channel_loads) {
        if (sample_cnt >= N_samples_up) {
            break;
        } else if (sample < min_cl) {
            min_cl = sample;
        }
        ++sample_cnt;
    }

    return std::isinf(min_cl) ? 0.0 : min_cl;
}

double StateMachine::max_channel_load() const
{
    assert(N_samples_down > 0);
    double max_cl = 0.0;
    std::size_t sample_cnt = 0;
    for (auto sample : m_channel_loads) {
        if (sample_cnt >= N_samples_down) {
            break;
        } else if (sample > max_cl) {
            max_cl = sample;
        }
        ++sample_cnt;
    }

    return max_cl;
}

} // namespace dcc
} // namespace vanetza
