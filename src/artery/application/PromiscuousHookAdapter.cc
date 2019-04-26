#include "PromiscuousHookAdapter.h"

namespace artery
{

PromiscuousHookAdapter::PromiscuousHookAdapter(NetworkInterface& interface, artery::PromiscuousHook* promiscuousHook) :
    mPromiscuousHook(promiscuousHook),
    mInterface(interface)
{
}

// vanetza::btp::PortDispatcher::PromiscuousHook
void PromiscuousHookAdapter::tap_packet(const vanetza::btp::DataIndication& indication, const vanetza::UpPacket& packet)
{
    mPromiscuousHook->tap_packet(indication, std::move(packet), mInterface);
}



} // namespace artery
