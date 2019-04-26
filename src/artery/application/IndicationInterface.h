#ifndef ARTERY_INDICATIONINTERFACE_H_
#define ARTERY_INDICATIONINTERFACE_H_

#include <vanetza/btp/data_indication.hpp>

namespace artery
{

class NetworkInterface;

class IndicationInterface
{
    public:
        virtual void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>, NetworkInterface&) = 0;
};

} // namespace artery

#endif
