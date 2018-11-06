#ifndef ARTERY_XMLMULTICHANNELPOLICY_H_RKGV4VNM
#define ARTERY_XMLMULTICHANNELPOLICY_H_RKGV4VNM

#include "artery/application/MultiChannelPolicy.h"
#include <omnetpp/cxmlelement.h>
#include <map>
#include <set>

namespace artery
{

/**
 * XmlMultiChannelPolicy reads the ITS-AID -> ITS-G5 channel mapping from XML
 */
class XmlMultiChannelPolicy : public MultiChannelPolicy
{
    public:
        XmlMultiChannelPolicy() = default;
        XmlMultiChannelPolicy(const omnetpp::cXMLElement*);

        void read(const omnetpp::cXMLElement*);

        std::vector<ChannelNumber> allChannels(vanetza::ItsAid aid) const override;

    private:
        std::map<vanetza::ItsAid, std::set<ChannelNumber>> mApplicationMapping;
        ChannelNumber mDefaultChannel;
};

} // namespace artery

#endif /* ARTERY_XMLMULTICHANNELPOLICY_H_RKGV4VNM */

