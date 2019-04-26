#ifndef ARTERY_MCOPROMISCUOUSHOOKADAPTER_H_
#define ARTERY_MCOPROMISCUOUSHOOKADAPTER_H_

#include "vanetza/btp/port_dispatcher.hpp"
#include "PromiscuousHook.h"

namespace artery
{

class PromiscuousHookAdapter : public vanetza::btp::PortDispatcher::PromiscuousHook
{
    public:
        PromiscuousHookAdapter(NetworkInterface&, artery::PromiscuousHook*);

        // input vanetza::btp::PortDispatcher::PromiscuousHook
        void tap_packet(const vanetza::btp::DataIndication&, const vanetza::UpPacket&) override final;

    private:
        // output interface
        artery::PromiscuousHook* mPromiscuousHook;

        NetworkInterface& mInterface;
};

} // namespace artery

#endif /* ARTERY_MCOPROMISCUOUSHOOKADAPTER_H_ */
