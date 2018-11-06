/*
* Artery V2X Simulation Framework
* Copyright 2015-2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_ITSG5PROMISCUOUSSERVICE_H_
#define ARTERY_ITSG5PROMISCUOUSSERVICE_H_

#include "artery/application/ItsG5BaseService.h"
#include "artery/application/TappingInterface.h"
#include <vanetza/btp/port_dispatcher.hpp>

namespace artery
{

/**
 * ItsG5PromiscuousService allows for listening on all BTP ports, i.e. tapping every received BTP packet.
 *
 * Inheriting classes are expected to override one of the tapPacket methods for grabbing packets.
 */
class ItsG5PromiscuousService : public ItsG5BaseService, public TappingInterface
{
    public:
        /**
         * Tap received BTP packet with information about receiving network interface
         */
        virtual void tap(const vanetza::btp::DataIndication&, const vanetza::UpPacket&, const NetworkInterface&) override;

        /**
         * Tap received BTP packet without information about receiving network interface
         * \deprecated This method is maintained for backward-compatibility, use tap() instead
         */
        virtual void tapPacket(const vanetza::btp::DataIndication&, const vanetza::UpPacket&) {}
};

} // namespace artery

#endif /* ARTERY_ITSG5PROMISCUOUSSERVICE_H_ */
