/*
* Artery V2X Simulation Framework
* Copyright 2020 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_RTCMMOCKRECEIVER_H_QAPFRBVY
#define ARTERY_RTCMMOCKRECEIVER_H_QAPFRBVY

#include "artery/application/ItsG5Service.h"

namespace artery
{

class RtcmMockReceiver : public artery::ItsG5Service
{
    protected:
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
};

} // namespace artery

#endif /* ARTERY_RTCMMOCKRECEIVER_H_QAPFRBVY */

