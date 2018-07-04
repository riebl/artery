#include "BlackIceWarner.h"
#include "lte_msgs/BlackIceWarning_m.h"
#include "artery/application/Middleware.h"
#include "artery/traci/VehicleController.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/packet/chunk/cPacketChunk.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <omnetpp/checkandcast.h>

using namespace omnetpp;

Define_Module(BlackIceWarner)

BlackIceWarner::~BlackIceWarner()
{
    cancelAndDelete(pollingTrigger);
}

void BlackIceWarner::initialize(int stage)
{
    if (stage == artery::InitStages::Prepare) {
        pollingRadius = par("pollingRadius");
        pollingInterval = par("pollingInterval");
        pollingTrigger = new cMessage("poll black ice central");

        numWarningsCentral = 0;
        WATCH(numWarningsCentral);
    } else if (stage == artery::InitStages::Self) {
        socket.setOutputGate(gate("udpOut"));
        auto centralAddress = inet::L3AddressResolver().resolve(par("centralAddress"));
        socket.connect(centralAddress, par("centralPort"));
        socket.setCallback(this);

        auto mw = inet::getModuleFromPar<artery::Middleware>(par("middlewareModule"), this);
        vehicleController = artery::notNullPtr(mw->getFacilities().get_mutable_ptr<traci::VehicleController>());

        scheduleAt(simTime() + uniform(0.0, pollingInterval), pollingTrigger);
    }
}

int BlackIceWarner::numInitStages() const
{
    return artery::InitStages::Total;
}

void BlackIceWarner::finish()
{
    socket.close();
    recordScalar("numWarningsCentral", numWarningsCentral);
}

void BlackIceWarner::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage()) {
        pollCentral();
    } else if (socket.belongsToSocket(msg)) {
        socket.processMessage(msg);
    } else {
        throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
    }
}

void BlackIceWarner::socketDataArrived(inet::UdpSocket*, inet::Packet* packet)
{
    auto chunk = packet->popAtFront<inet::cPacketChunk>();
    processResponse(*check_and_cast<BlackIceResponse*>(chunk->getPacket()));
    delete packet;
}

void BlackIceWarner::socketErrorArrived(inet::UdpSocket*, inet::Indication* indication)
{
    EV_ERROR << "socket error occurred: " << indication;
    delete indication;
}

void BlackIceWarner::pollCentral()
{
    auto query = new BlackIceQuery("poll for black ice");
    query->setPositionX(vehicleController->getPosition().x / boost::units::si::meter);
    query->setPositionY(vehicleController->getPosition().y / boost::units::si::meter);
    query->setRadius(pollingRadius);
    auto packet = new inet::Packet(query->getName());
    packet->insertAtFront(inet::makeShared<inet::cPacketChunk>(query));
    socket.send(packet);
    scheduleAt(simTime() + pollingInterval, pollingTrigger);
}

void BlackIceWarner::processResponse(BlackIceResponse& response)
{
    EV_INFO << "Black ice warnings: " << response.getWarnings() << "\n";
    if (response.getWarnings() >= 2 && !reducedSpeed) {
        vehicleController->setSpeedFactor(0.5);
        reducedSpeed = true;
        ++numWarningsCentral;
    } else if (response.getWarnings() == 0 && reducedSpeed) {
        vehicleController->setSpeedFactor(1.0);
        reducedSpeed = false;
    }
}
