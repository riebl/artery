#include "artery/networking/GeoNetRequest.h"
#include "artery/testbed/CubeConnection.h"
#include "artery/testbed/OtaInterfaceCube.h"
#include "artery/testbed/OtaInterfaceLayer.h"
#include "artery/testbed/TestbedScheduler.h"
#include <omnetpp/cwatch.h>

namespace artery
{

Define_Module(OtaInterfaceCube)

void OtaInterfaceCube::initialize(int stage)
{
    if (stage == 0) {
        WATCH(mPacketsForwarded);
        WATCH(mPacketsInjected);

        // try fetching required TestbedScheduler
        auto scheduler = dynamic_cast<TestbedScheduler*>(omnetpp::getSimulation()->getScheduler());
        if(!scheduler) {
            throw omnetpp::cRuntimeError("OtaInterfaceCube requires TestbedScheduler!");
        }

        // build indication queue and wire it up with scheduler
        mOtaIndicationQueue = std::make_unique<OtaIndicationQueue>(this);
        scheduler->setOtaIndicationQueue(mOtaIndicationQueue);

        // set up connection to CUBE
        auto inject = [this](const vanetza::access::DataRequest& request, std::unique_ptr<vanetza::CohesivePacket> payload) {
            std::unique_ptr<GeoNetPacket> gn(new GeoNetPacket("GeoNet packet from CUBE"));
            gn->setPayload(std::move(payload));
            gn->setControlInfo(new GeoNetRequest(request));
            // handler is likely called from another thread, but OtaIndicationQueue is thread-safe
            mOtaIndicationQueue->trigger(std::move(gn));
        };
        boost::asio::ip::address cube_addr = boost::asio::ip::make_address(par("cubeAddress").stringValue());
        mCubeConnection = std::make_unique<CubeConnection>(cube_addr, par("cubeRxPort"), par("cubeTxPort"), inject);
    }
}

void OtaInterfaceCube::registerModule(OtaInterfaceLayer* module)
{
    if (mRegisteredModule) {
        omnetpp::cRuntimeError("OTA Interface: A module is already registered!");
    } else {
        mRegisteredModule = module;
    }
}

void OtaInterfaceCube::unregisterModule()
{
    mRegisteredModule = nullptr;
}

int OtaInterfaceCube::numInitStages() const
{
    return 1;
}

void OtaInterfaceCube::sendMessage(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data)
{
    // forward intercepted message to CUBE
    if (mCubeConnection) {
        ++mPacketsForwarded;
        mCubeConnection->sendPacket(source, destination, data);
    }
}

void OtaInterfaceCube::finish()
{
    mCubeConnection.reset();
}

void OtaInterfaceCube::receiveMessage(std::unique_ptr<GeoNetPacket> geonetPacket)
{
    if (hasRegisteredModule()) {
        ++mPacketsInjected;
        mRegisteredModule->request(std::move(geonetPacket));
    }
}

bool OtaInterfaceCube::hasRegisteredModule()
{
    return mRegisteredModule != nullptr;
}

} // namespace artery
