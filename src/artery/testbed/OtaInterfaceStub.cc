#include "artery/testbed/OtaInterfaceStub.h"
#include "artery/testbed/OtaIndicationQueue.h"
#include "artery/testbed/TestbedScheduler.h"

namespace artery
{

Define_Module(OtaInterfaceStub)

void OtaInterfaceStub::initialize()
{
    auto scheduler = dynamic_cast<TestbedScheduler*>(omnetpp::getSimulation()->getScheduler());
    if (scheduler) {
        scheduler->setOtaIndicationQueue(std::make_shared<OtaIndicationQueue>(this));
    } else {
        EV_INFO << "No OtaIndicationQueue passed to scheduler";
    }
}

void OtaInterfaceStub::registerModule(OtaInterfaceLayer* layer)
{
    mRegisteredModule = layer;
}

void OtaInterfaceStub::unregisterModule()
{
    mRegisteredModule = nullptr;
}

void OtaInterfaceStub::sendMessage(const vanetza::MacAddress&, const vanetza::MacAddress&, const vanetza::byte_view_range&)
{
    //nothing to do here, as no hardware is connected
}

void OtaInterfaceStub::receiveMessage(std::unique_ptr<GeoNetPacket>)
{
}

} // namespace artery
