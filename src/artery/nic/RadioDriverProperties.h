#ifndef RADIODRIVERPROPERTIES_H_4GALJBTN
#define RADIODRIVERPROPERTIES_H_4GALJBTN

#include <vanetza/net/mac_address.hpp>
#include <omnetpp/cmessage.h>

namespace artery
{

struct RadioDriverProperties : public omnetpp::cMessage
{
    vanetza::MacAddress LinkLayerAddress;
};

} // namespace artery

#endif /* RADIODRIVERPROPERTIES_H_4GALJBTN */

