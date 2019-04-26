#ifndef ARTERY_NETWORKINTERFACE_H_
#define ARTERY_NETWORKINTERFACE_H_

#include "artery/networking/FsmDccEntity.h"
#include "artery/networking/Router.h"
#include "artery/utility/Channel.h"
#include <vanetza/btp/port_dispatcher.hpp>


namespace artery
{

class ItsG5BaseService;

class NetworkInterface
{
    public:
        using port_type = uint16_t;

        Channel channel;
        Router* router;
        IDccEntity* dccEntity;

        vanetza::btp::PortDispatcher btpPortDispatcher;

        NetworkInterface(Router* router, IDccEntity* entity) :
            channel(router->getChannel())
            , router(router)
            , dccEntity(entity)
        {
        }

        vanetza::dcc::TransmitRateThrottle* getTransmitRateThrottle() const
        {
            if (!dccEntity) {
                throw omnetpp::cRuntimeError("No IDccEntity");
            }
            return dccEntity->getTransmitRateThrottle();
        }

        vanetza::dcc::ChannelProbeProcessor* getChannelProbeProcessor() const
        {
            if (!dccEntity) {
                throw omnetpp::cRuntimeError("No IDccEntity");
            }
            return dccEntity->getChannelProbeProcessor();
        }

        vanetza::dcc::StateMachine* getStateMachine() const
        {
            auto* fsmEntity {dynamic_cast<FsmDccEntity*>(dccEntity)};
            if (fsmEntity) {
                return fsmEntity->getStateMachine();
            } else {
                throw omnetpp::cRuntimeError("No FsmDccEntity found");
            }
        }
};

} // namespace artery

#endif /* ARTERY_NETWORKINTERFACE_H_ */
