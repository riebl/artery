#include "ChannelLoadMeasurements.h"
#include <csimulation.h>
#include <algorithm>

ChannelLoadMeasurements::ChannelLoadMeasurements() :
    m_samples(12500), m_busy(false)
{
}

void ChannelLoadMeasurements::reset()
{
    m_samples.clear();
    m_last_update = simTime();
    m_busy = false;
}

void ChannelLoadMeasurements::busy()
{
    fill(true);
}

void ChannelLoadMeasurements::idle()
{
    fill(false);
}

void ChannelLoadMeasurements::fill(bool busy)
{
    const simtime_t now = simTime();
    const simtime_t duration = now - m_last_update;

    const std::size_t samples = duration / simtime_t { 8, SIMTIME_US };
    for (std::size_t i = 0; i < samples; ++i) {
        m_samples.push_back(m_busy);
    }

    m_busy = busy;
    m_last_update = now;
}

vanetza::dcc::ChannelLoad ChannelLoadMeasurements::channel_load()
{
    fill(m_busy);
    const unsigned busy = std::count(m_samples.begin(), m_samples.end(), true);
    const unsigned total = m_samples.capacity();
    return { busy, total };
}
