/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_VANETRADIO_H_FNQDI1V8
#define ARTERY_VANETRADIO_H_FNQDI1V8

#include "artery/nic/ChannelLoadSampler.h"
#include "inet/physicallayer/ieee80211/packetlevel/Ieee80211Radio.h"

namespace artery
{

/**
 * Specialised INET-based IEEE 802.11 radio for VANET communication.
 *
 * - emit RadioFrame signal on each incoming radio frame for CBR measurements
 */
class VanetRadio : public inet::physicallayer::Ieee80211Radio
{
public:
    static const omnetpp::simsignal_t RadioFrameSignal;

protected:
    void handleLowerPacket(inet::physicallayer::RadioFrame*) override;
};

} // namespace artery

#endif /* ARTERY_VANETRADIO_H_FNQDI1V8 */

