#include "IndicationInterfaceAdapter.h"
#include <boost/optional/optional_io.hpp>

namespace artery
{

IndicationInterfaceAdapter::IndicationInterfaceAdapter(NetworkInterface& interface, artery::IndicationInterface* indicationInterface) :
    mIndicationInterface(indicationInterface),
    mInterface(interface)
{
}

// vanetza::btp::IndicationInterface
void IndicationInterfaceAdapter::indicate(const vanetza::btp::DataIndication& btp_ind, std::unique_ptr<vanetza::UpPacket> packet)
{
    mIndicationInterface->indicate(btp_ind, std::move(packet), mInterface);
}

} // namespace artery
