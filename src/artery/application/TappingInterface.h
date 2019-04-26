#ifndef ARTERY_TAPPINGINTERFACE_H_HK2Z6K0S
#define ARTERY_TAPPINGINTERFACE_H_HK2Z6K0S

#include <vanetza/btp/data_indication.hpp>

namespace artery
{

class NetworkInterface;

/**
 * TappingInterface needs to get implemented by all "promiscuous" packet listeners
 */
class TappingInterface
{
    public:
        virtual void tap(const vanetza::btp::DataIndication&, const vanetza::UpPacket&, const NetworkInterface&) = 0;

        virtual ~TappingInterface() = default;
};

} // namespace artery

#endif /* ARTERY_TAPPINGINTERFACE_H_HK2Z6K0S */
