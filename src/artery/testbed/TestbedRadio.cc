#include "artery/testbed/TestbedRadio.h"
#include <inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h>
#include <inet/physicallayer/common/packetlevel/RadioMedium.h>
#include <inet/physicallayer/common/packetlevel/Radio.h>

using namespace omnetpp;

namespace artery
{

Define_Module(TestbedRadio)

void TestbedRadio::initialize(int stage)
{
    Ieee80211Radio::initialize(stage);
    mReachableTime = par("reachableTime");
}

void TestbedRadio::sendUp(inet::Packet* packet)
{
    const auto& header = packet->peekAtFront<inet::ieee80211::Ieee80211DataHeader>(inet::b(-1), inet::Chunk::PF_ALLOW_NULLPTR);
    if (header)
        mReachableNodes[header->getTransmitterAddress()] = simTime();
    Radio::sendUp(packet);
}

void TestbedRadio::updateReachableNodes()
{
    for (auto reachableNode = mReachableNodes.begin(); reachableNode != mReachableNodes.end();) {
        if ((simTime() - reachableNode->second) > mReachableTime) {
            reachableNode = mReachableNodes.erase(reachableNode);
        } else {
            ++reachableNode;
        }
    }
}

const std::map<inet::MacAddress, omnetpp::simtime_t>& TestbedRadio::getReachableNodes()
{
    updateReachableNodes();
    return mReachableNodes;
}

} // namespace artery
