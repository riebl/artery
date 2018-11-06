#ifndef MCOSTRATEGY_H
#define MCOSTRATEGY_H

#include "artery/application/NetworkInterfaceTable.h"

namespace artery
{

class McoStrategy
{
    public:
        McoStrategy(NetworkInterfaceTable& table);

        NetworkInterfaceTable::InterfaceList choose(const vanetza::ItsAid its_aid);

        void add(vanetza::ItsAid aid, Channel channel);

    private:
        NetworkInterfaceTable& mTable;
        std::multimap<const vanetza::ItsAid, const Channel> mAidMap;
};

} // namespace artery

#endif /* MCOSTRATEGY_H */

