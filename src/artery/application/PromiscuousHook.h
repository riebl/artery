#ifndef ARTERY_MCOPROMISCUOUSHOOK_H_
#define ARTERY_MCOPROMISCUOUSHOOK_H_

#include <vanetza/btp/data_indication.hpp>

namespace artery
{

class NetworkInterface;

class PromiscuousHook
{
    public:
        virtual void tap_packet(const vanetza::btp::DataIndication&, const vanetza::UpPacket&, NetworkInterface&) = 0;
};

} // namespace artery

#endif /* ARTERY_MCOPROMISCUOUSHOOK_H_ */
