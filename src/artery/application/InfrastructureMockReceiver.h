/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_INFRASTRUCTUREMOCKRECEIVER_H_3JS8PA4K
#define ARTERY_INFRASTRUCTUREMOCKRECEIVER_H_3JS8PA4K

#include "artery/application/ItsG5Service.h"

namespace artery
{

class InfrastructureMockReceiver : public artery::ItsG5Service
{
    protected:
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
};

} // namespace artery

#endif /* ARTERY_INFRASTRUCTUREMOCKRECEIVER_H_3JS8PA4K */

