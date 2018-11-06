#include "artery/application/MultiChannelPolicy.h"
#include <algorithm>

namespace artery
{

ChannelNumber MultiChannelPolicy::primaryChannel(vanetza::ItsAid aid) const
{
    auto channels = allChannels(aid);
    std::sort(channels.begin(), channels.end(), ChannelRankCompare);
    return !channels.empty() ? channels.front() : 0;
}

} // namespace artery
