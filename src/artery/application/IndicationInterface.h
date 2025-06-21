#ifndef ARTERY_INDICATIONINTERFACE_H_
#define ARTERY_INDICATIONINTERFACE_H_

#include <vanetza/btp/data_indication.hpp>
#include <vanetza/net/packet.hpp>

namespace artery
{

class NetworkInterface;

/**
 * IndicationInterface has to be implemented by any entity receiving ITS-G5 messages.
 */
class IndicationInterface
{
    public:
        virtual void indicate(const vanetza::btp::DataIndication&, std::unique_ptr<vanetza::UpPacket>, const NetworkInterface&) = 0;

        virtual ~IndicationInterface() = default;
};

} // namespace artery

#endif
