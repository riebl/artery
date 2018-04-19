#include "artery/testbed/TestbedContention.h"

namespace artery
{

Define_Module(TestbedContention)

void TestbedContention::initialize(int stage)
{
    inet::ieee80211::Contention::initialize(stage);
    if (stage == 0) {
        mIfs = par("interframeSpacing");
    }
}

void TestbedContention::startContention(int cw, simtime_t ifs, simtime_t eifs, simtime_t slotTime, inet::ieee80211::Contention::ICallback *callback)
{
    inet::ieee80211::Contention::startContention(0, mIfs, eifs, slotTime, callback);
}

} // namespace artery
