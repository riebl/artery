#include "artery/application/McoStrategy.h"

namespace artery
{
    McoStrategy::McoStrategy(NetworkInterfaceTable& table) : mTable(table) {}

    NetworkInterfaceTable::InterfaceList McoStrategy::choose(const vanetza::ItsAid its_aid)
    {
        auto channels = mAidMap.equal_range(its_aid);
        NetworkInterfaceTable::InterfaceList interfaces;
        for (auto channel : boost::make_iterator_range(channels.first, channels.second)) {
            auto ifcs = mTable.getInterfaceByChannel(channel.second);
            for (auto& ifc : ifcs) {
                interfaces.push_back(ifc.get());
            }
        }
        return interfaces;

    }

    void McoStrategy::add(vanetza::ItsAid aid, Channel channel)
    {
        mAidMap.emplace(aid, channel);
    }

} // namespace artery


