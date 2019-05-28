#ifndef RADIODRIVERPROPERTIES_H_4GALJBTN
#define RADIODRIVERPROPERTIES_H_4GALJBTN

#include "artery/utility/Channel.h"
#include <vanetza/net/mac_address.hpp>
#include <omnetpp/cmessage.h>

namespace artery
{

struct RadioDriverProperties : public omnetpp::cMessage
{
    vanetza::MacAddress LinkLayerAddress;
    ChannelNumber ServingChannel;
};

} // namespace artery

#endif /* RADIODRIVERPROPERTIES_H_4GALJBTN */

