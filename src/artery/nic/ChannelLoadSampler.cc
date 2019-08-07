#include "artery/nic/ChannelLoadSampler.h"
#include <omnetpp/csimulation.h>
#include <chrono>

namespace artery
{

ChannelLoadSampler::ChannelLoadSampler() : mBusy(false), mCbr(0.0)
{
    reset();
}

void ChannelLoadSampler::reset()
{
    mLastUpdate = omnetpp::simTime();
    mSamples.clear();
    mBusy = false;
    mCbr = 0.0;
}

void ChannelLoadSampler::busy(bool flag)
{
    if (mBusy != flag) {
        const auto fillSamples = computePendingSamples();
        if (fillSamples > 0) {
            // fill if busy state changed for at least one sample
            mSamples.emplace_front(fillSamples, mBusy);
        }
        mBusy = flag;
        mLastUpdate = omnetpp::simTime();
    }
}

unsigned ChannelLoadSampler::computePendingSamples() const
{
    static const omnetpp::SimTime cbrSamplePeriod { 8, omnetpp::SIMTIME_US };

    const auto updateDelta = omnetpp::simTime() - mLastUpdate;
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
        busy = mBusy ? samples : 0;
    }

    // iterate recorded busy periods (begin with most recent period)
    for (auto it = mSamples.begin(); it != mSamples.end();) {
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
            it = mSamples.erase(it, mSamples.end());
        }
    }

    mCbr = static_cast<double>(busy) / static_cast<double>(cbrIntervalSamples);
}

double ChannelLoadSampler::cbr()
{
    updateCbr();
    return mCbr;
}

std::ostream& operator<<(std::ostream& os, const ChannelLoadSampler& sampler)
{
    os << "CBR=" << sampler.mCbr << " (" << sampler.mSamples.size() << " busy edges pending)";
    return os;
}

} // namespace artery
