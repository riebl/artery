#include "artery/application/NetworkInterfaceTable.h"

namespace artery
{

std::shared_ptr<NetworkInterface> NetworkInterfaceTable::select(ChannelNumber ch) const
{
    std::shared_ptr<NetworkInterface> network;
    for (const auto interface : mInterfaces) {
        if (interface->channel == ch) {
            network = interface;
            break;
        }
    }

    return network;
};

const NetworkInterfaceTable::TableContainer& NetworkInterfaceTable::all() const
{
    return mInterfaces;
}

void NetworkInterfaceTable::insert(std::shared_ptr<NetworkInterface> ifc)
{
    mInterfaces.insert(ifc);
}

} // namespace artery
