#include "artery/messages/GeoNetPacket_m.h"
#include "artery/nic/RadioDriverBase.h"
#include "artery/netw/GeoNetIndication.h"
#include "artery/testbed/OtaInterfaceLayer.h"
#include "artery/testbed/OtaInterface.h"
#include "artery/testbed/TestbedRadio.h"
#include "artery/traci/ControllableVehicle.h"
#include "artery/traci/VehicleController.h"
#include <inet/common/ModuleAccess.h>
#include <vanetza/net/packet_variant.hpp>
#include <inet/physicallayer/contract/packetlevel/IRadio.h>

namespace artery
{

Define_Module(OtaInterfaceLayer)

void OtaInterfaceLayer::initialize(int stage)
{
    if (stage == 1) {
        std::string otaInterfaceModule = par("otaInterfaceModule");
        mOtaModule = dynamic_cast<OtaInterface*>(getModuleByPath(otaInterfaceModule.c_str()));
        if (!mOtaModule) {
            throw omnetpp::cRuntimeError(this, "Specified OTA module %s not found! "
                    "Check if withTestbed was set to true", otaInterfaceModule.c_str());
        }
        mOtaModule->registerModule(this);
        mRadioDriverIn = gate("lowerLayerIn");
        mRadioDriverOut = gate("lowerLayerOut");

        auto mobility = inet::getModuleFromPar<ControllableVehicle>(par("mobilityModule"), this);
        mVehicleController = mobility->getVehicleController();
        ASSERT(mVehicleController);
    }
}

void OtaInterfaceLayer::finish()
{
    mOtaModule->unregisterModule();
}

void OtaInterfaceLayer::handleMessage(omnetpp::cMessage* message)
{
    if (message->getArrivalGate() == mRadioDriverIn) {
        auto packet = check_and_cast<GeoNetPacket*>(message);
        auto info = check_and_cast<GeoNetIndication*>(message->removeControlInfo());

        auto payload = packet->getPayload().extract_up_packet();
        size_t length = boost::size(*payload, vanetza::OsiLayer::Physical, vanetza::OsiLayer::Application);
        auto range = boost::create_byte_view(*payload, vanetza::OsiLayer::Network);

        if (length != range.size()) {
            throw omnetpp::cRuntimeError(this, "Whole packet must be sent to OTA Interface");
        }

        if (info) {
            mOtaModule->sendMessage(info->source, info->destination, range);
        }
    }

    cancelAndDelete(message);
}

void OtaInterfaceLayer::request(std::unique_ptr<GeoNetPacket> packet)
{
    Enter_Method("request");
    GeoNetPacket* ptr = packet.release();
    take(ptr);
    send(ptr, mRadioDriverOut);
}

GeoPosition OtaInterfaceLayer::getCurrentPosition()
{
    return mVehicleController->getGeoPosition();
}

vanetza::units::Velocity OtaInterfaceLayer::getCurrentSpeed()
{
    return mVehicleController->getSpeed();
}

Angle OtaInterfaceLayer::getCurrentHeading()
{
    return mVehicleController->getHeading();
}

} // namespace artery
