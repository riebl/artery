#ifndef ARTERY_INDICATIONINTERFACEADAPTER_H_
#define ARTERY_INDICATIONINTERFACEADAPTER_H_

#include "vanetza/btp/data_interface.hpp"
#include "IndicationInterface.h"

namespace artery
{

class IndicationInterfaceAdapter : public vanetza::btp::IndicationInterface
{
    public:
        IndicationInterfaceAdapter(NetworkInterface& interface, artery::IndicationInterface* indicationInterface);

        // input vanetza::btp::IndicationInterface
        void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>) override;

    private:
        // output interface
        artery::IndicationInterface* mIndicationInterface;

        NetworkInterface& mInterface;
};


} // namespace artery

#endif
