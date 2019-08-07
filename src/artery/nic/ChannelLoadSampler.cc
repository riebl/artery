#include "artery/nic/ChannelLoadSampler.h"
#include <omnetpp/csimulation.h>
#include <chrono>

namespace artery
{

ChannelLoadSampler::ChannelLoadSampler() : m_busy(false), m_cbr(0.0)
{
    reset();
}

void ChannelLoadSampler::reset()
{
    m_last_update = omnetpp::simTime();
    m_samples.clear();
    m_busy = false;
    m_cbr = 0.0;
}

void ChannelLoadSampler::busy(bool flag)
{
    if (m_busy != flag) {
        const auto fillSamples = computePendingSamples();
        if (fillSamples > 0) {
            // fill if busy state changed for at least one sample
            m_samples.emplace_front(fillSamples, m_busy);
        }
        m_busy = flag;
        m_last_update = omnetpp::simTime();
    }
}

unsigned ChannelLoadSampler::computePendingSamples() const
{
    static const omnetpp::SimTime cbrSamplePeriod { 8, omnetpp::SIMTIME_US };

    const auto updateDelta = omnetpp::simTime() - m_last_update;
    return updateDelta / cbrSamplePeriod;
}

void ChannelLoadSampler::updateCbr()
{
    static const unsigned cbrIntervalSamples = 12500;

    unsigned samples = 0;
    unsigned busy = 0;

    // consider samples since last busy state change
    const auto pendingSamples = computePendingSamples();
    if (pendingSamples > 0) {
        samples = std::min(cbrIntervalSamples, pendingSamples);
        busy = m_busy ? samples : 0;
    }

    // iterate recorded busy periods (begin with most recent period)
    for (auto it = m_samples.begin(); it != m_samples.end();) {
        // get values from tuples
        unsigned sampleLength = std::get<0>(*it);
        bool mediumBusy = std::get<1>(*it);

        // add length of list entry to total samples
        samples += sampleLength;

        if (samples < cbrIntervalSamples) {
            if (mediumBusy) {
                // increase busy time by full sample length
                busy += sampleLength;
            }
            ++it;
        } else {
            if (mediumBusy) {
                // increase busy time up to boundary of sampling interval
                busy += sampleLength - (samples - cbrIntervalSamples);
            }

            // remove obsolete periods
            it = m_samples.erase(it, m_samples.end());
        }
    }

    m_cbr = static_cast<double>(busy) / static_cast<double>(cbrIntervalSamples);
}

double ChannelLoadSampler::cbr()
{
    updateCbr();
    return m_cbr;
}

} // namespace artery
