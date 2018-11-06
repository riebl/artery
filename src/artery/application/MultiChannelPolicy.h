#ifndef ARTERY_MULTICHANNELPOLICY_H_JO180PV9
#define ARTERY_MULTICHANNELPOLICY_H_JO180PV9

#include "artery/utility/Channel.h"
#include <vanetza/common/its_aid.hpp>
#include <vector>

namespace artery
{

/**
 * MultiChannelPolicy is an interface mapping ITS-AIDs to ITS-G5 channels
 */
class MultiChannelPolicy
{
    public:
        /**
         * Get all channels to which messages of ITS-AID are to be sent.
         *
         * \param aid ITS-AID
         * \return list of channel number (can be empty)
         */
        virtual std::vector<ChannelNumber> allChannels(vanetza::ItsAid aid) const = 0;

        /**
         * Get primary channel for a particular ITS-AID.
         * By default, primary channel is selected based on the channel rank,
         * i.e. CCH is preferred over all other channels if available
         *
         * \param aid ITS-AID
         * \return primary channel number (0 if no channel fits)
         */
        virtual ChannelNumber primaryChannel(vanetza::ItsAid aid) const;

        virtual ~MultiChannelPolicy() = default;
};

} // namespace artery

#endif /* ARTERY_MULTICHANNELPOLICY_H_JO180PV9 */

