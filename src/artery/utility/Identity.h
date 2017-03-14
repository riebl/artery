/*
 * Artery V2X Simulation Framework
 * Copyright 2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef IDENTITY_H_WXAWFSP2
#define IDENTITY_H_WXAWFSP2

#include <omnetpp/cobject.h>
#include <vanetza/geonet/address.hpp>
#include <vanetza/net/mac_address.hpp>
#include <cstdint>
#include <string>

namespace artery
{

class Identity : public omnetpp::cObject
{
public:
    std::string traci; /*< Vehicle ID used by TraCI protocol */
    uint32_t application; /*< ETSI station ID */
    vanetza::geonet::Address geonet; /*< GeoNetworking layer */
};

} // namespace artery

#endif /* IDENTITY_H_WXAWFSP2 */

