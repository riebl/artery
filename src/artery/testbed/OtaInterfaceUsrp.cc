#include "artery/netw/GeoNetRequest.h"
#include "artery/testbed/OtaInterfaceUsrp.h"
#include "artery/testbed/OtaInterfaceLayer.h"
#include "artery/testbed/TestbedScheduler.h"
#include "traci/Angle.h"
#include <sea_v2x/types.h>
#include <vanetza/access/data_request.hpp>
#include <vanetza/common/byte_order.hpp>
#include <vanetza/geonet/packet.hpp>

namespace artery
{
namespace
{
const auto traciStepSignal = omnetpp::cComponent::registerSignal("traci.step");

vanetza::AccessCategory mapAccessCategory(sea_v2x::ServiceClassType sc)
{
    vanetza::AccessCategory ac = vanetza::AccessCategory::BK;
    switch (sc) {
        case sea_v2x::ServiceClassType::AC_BE:
            ac = vanetza::AccessCategory::BE;
            break;
        case sea_v2x::ServiceClassType::AC_BK:
            ac = vanetza::AccessCategory::BK;
            break;
        case sea_v2x::ServiceClassType::AC_VI:
            ac = vanetza::AccessCategory::VI;
            break;
        case sea_v2x::ServiceClassType::AC_VO:
            ac = vanetza::AccessCategory::VO;
            break;
        default:
            throw omnetpp::cRuntimeError("Unknown ServiceClassType");
    }

    return ac;
}

} // namespace

Define_Module(OtaInterfaceUsrp)

void OtaInterfaceUsrp::initialize(int stage)
{
    if(stage == 0) {
        mFakeMode = par("fakeMode");

        mOtaIndicationQueue.reset(new OtaIndicationQueue(this));
        auto scheduler = dynamic_cast<TestbedScheduler*>(omnetpp::getSimulation()->getScheduler());
        if(!scheduler) {
            throw omnetpp::cRuntimeError("Testbed requires TestbedScheduler!");
        }
        scheduler->setOtaIndicationQueue(mOtaIndicationQueue);

        getSimulation()->getSystemModule()->subscribe("traci.step", this);

    } else if(stage == 1) {
        UsrpConnection::SeaConnectionParams params;
        params.hcmIP = par("hcmIp").stringValue();
        params.hcmPort = par("hcmPort");
        params.listeningPort = par("listeningPort");
        params.connectTimeout = par("connectTimeout");
        mConnection.reset(new UsrpConnection(params, this));
        testUsrpConnection();

        if (par("openGpsdSocket").boolValue()) {
            mGpsdServer.reset(new GpsdServer(par("datetime"), par("gpsdPort")));
            mGpsdUpdateRate = par("gpsdUpdateRate").doubleValue();
            mGpsdTimer = new omnetpp::cMessage("GPSD timer");
        }
    }
}

void OtaInterfaceUsrp::testUsrpConnection()
{
    if (mFakeMode) {
        static const char* warning = "OTA interface USRP fake does never connect to any USRP hardware\n";
        EV_WARN << warning;
        std::cout << warning;
    } else if (!mConnection->isConnectionOk() && mFakeMode) {
        mConnection->shutDownConnection();
        throw omnetpp::cRuntimeError(this, "Could not connect to USRP");
    }
}

void OtaInterfaceUsrp::registerModule(OtaInterfaceLayer* module)
{
    if (mRegisteredModule) {
        omnetpp::cRuntimeError("OTA Interface: Module registered twice!");
    } else {
        mRegisteredModule = module;
        if (mGpsdTimer) {
            scheduleAt(mGpsdUpdateRate + omnetpp::simTime(), mGpsdTimer);
        }
    }
}

void OtaInterfaceUsrp::unregisterModule()
{
    mRegisteredModule = nullptr;
    cancelAndDelete(mGpsdTimer);
}

int OtaInterfaceUsrp::numInitStages() const
{
    return 2;
}

void OtaInterfaceUsrp::sendMessage(const vanetza::MacAddress& source, const vanetza::MacAddress& destination, const vanetza::byte_view_range& data)
{
    ++mMessagesToDut;
    mConnection->sendPacket(source, destination, data);
}

void OtaInterfaceUsrp::finish()
{
    EV_INFO << "Messages from DUT: " << mMessagesFromDut << std::endl;
    EV_INFO << "Messages to DUT: " << mMessagesToDut << std::endl;
    mConnection->shutDownConnection();
}

void OtaInterfaceUsrp::receiveSignal(omnetpp::cComponent* source, omnetpp::simsignal_t signalId, const omnetpp::simtime_t& simtime, omnetpp::cObject* node)
{
    if (signalId == traciStepSignal) {
        // each traci.step signal a new list with reachable nodes can be fetched
        // TODO: reachable nodes are required for real unicast support
    }
}

void OtaInterfaceUsrp::receiveMessage(std::unique_ptr<GeoNetPacket> geonetPacket)
{
    if (hasRegisteredModule()) {
        Enter_Method("receiveMessage");
        ++mMessagesFromDut;
        mRegisteredModule->request(std::move(geonetPacket));
    }
}

void OtaInterfaceUsrp::notifyQueue(const sea_v2x::MA_UNITDATA_indication& ind)
{
    vanetza::access::DataRequest req;
    req.access_category = mapAccessCategory(ind.service_class);
    req.destination_addr.octets = ind.destination_address;
    req.source_addr.octets = ind.source_address;
    req.ether_type = vanetza::uint16be_t(0x8947); // GeoNet EtherType

    std::unique_ptr<vanetza::geonet::UpPacket> payload {
        new vanetza::geonet::UpPacket {
            vanetza::CohesivePacket(ind.data, vanetza::OsiLayer::Network)
        }
    };

    std::unique_ptr<GeoNetPacket> gn(new GeoNetPacket("GeoNet packet from DUT"));
    gn->setPayload(std::move(payload));
    gn->setControlInfo(new GeoNetRequest(req));
    mOtaIndicationQueue->trigger(std::move(gn));
}

void OtaInterfaceUsrp::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == mGpsdTimer) {
        if (mGpsdServer && mRegisteredModule) {
            mGpsdServer->sendPositionFix(*mRegisteredModule);
        }
        scheduleAt(omnetpp::simTime() + mGpsdUpdateRate, mGpsdTimer);
    }
}

bool OtaInterfaceUsrp::hasRegisteredModule()
{
    return mRegisteredModule != nullptr;
}

} // namespace artery
