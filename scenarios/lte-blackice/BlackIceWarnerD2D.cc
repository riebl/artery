#include "BlackIceWarnerD2D.h"
#include "lte_msgs/BlackIceWarning_m.h"
#include "artery/application/Middleware.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/traci/VehicleController.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <omnetpp/checkandcast.h>

using namespace omnetpp;

Define_Module(BlackIceWarnerD2D)

static const simsignal_t storyboardSignal = cComponent::registerSignal("StoryboardSignal");

BlackIceWarnerD2D::~BlackIceWarnerD2D()
{
    cancelAndDelete(removeSpeedReduction);
}

int BlackIceWarnerD2D::numInitStages() const
{
    return inet::NUM_INIT_STAGES;
}

void BlackIceWarnerD2D::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    if (stage != inet::INITSTAGE_APPLICATION_LAYER) {
        return;
    }

    mcastAddress = inet::L3AddressResolver().resolve(par("mcastAddress"));
    mcastPort = par("mcastPort");
    socket.setOutputGate(gate("udpOut"));
    socket.bind(mcastPort);

    // LTE multicast support
    inet::IInterfaceTable *ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
    inet::InterfaceEntry *ie = ift->getInterfaceByName("wlan");
    if (!ie)
        throw cRuntimeError("Wrong multicastInterface setting: no interface named wlan");
    socket.setMulticastOutputInterface(ie->getInterfaceId());
    socket.joinMulticastGroup(mcastAddress);

    // application's supporting code
    auto mw = inet::getModuleFromPar<artery::Middleware>(par("middlewareModule"), this);
    mw->subscribe(storyboardSignal, this);
    vehicleController = artery::notNullPtr(mw->getFacilities().get_mutable_ptr<traci::VehicleController>());

    // application logic
    removeSpeedReduction = new omnetpp::cMessage("remove speed reduction");
    warningRadius = par("warningRadius");
    warningDuration = par("warningDuration");
    numWarningsPeer = 0;
    WATCH(numWarningsPeer);
    tractionLosses = 0;
    WATCH(tractionLosses);
}

void BlackIceWarnerD2D::finish()
{
    socket.close();
    recordScalar("numWarningsPeer", numWarningsPeer);
    recordScalar("traction losses", tractionLosses);
}

void BlackIceWarnerD2D::receiveSignal(cComponent*, simsignal_t sig, cObject* obj, cObject*)
{
    if (sig == storyboardSignal) {
        auto sigobj = check_and_cast<artery::StoryboardSignal*>(obj);
        if (sigobj->getCause() == "traction loss") {
            ++tractionLosses;
            sendReport();
        }
    }
}

void BlackIceWarnerD2D::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage()) {
        vehicleController->setSpeedFactor(1.0);
    } else if (msg->getKind() == inet::UDP_I_DATA) {
        processReport(*check_and_cast<BlackIceReport*>(msg));
        delete msg;
    } else {
        throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
    }
}

void BlackIceWarnerD2D::sendReport()
{
    Enter_Method_Silent();
    using boost::units::si::meter;
    using boost::units::si::meter_per_second;
    auto report = new BlackIceReport("black ice warning");
    report->setPositionX(vehicleController->getPosition().x / meter);
    report->setPositionY(vehicleController->getPosition().y / meter);
    report->setSpeed(vehicleController->getSpeed() / meter_per_second);
    report->setTime(simTime());
    socket.sendTo(report, mcastAddress, par("mcastPort"));
}

void BlackIceWarnerD2D::processReport(BlackIceReport& report)
{
    using boost::units::si::meter;
    double dx = report.getPositionX() - vehicleController->getPosition().x / meter;
    double dy = report.getPositionY() - vehicleController->getPosition().y / meter;
    if (dx * dx + dy * dy > warningRadius * warningRadius) {
        return;
    }

    ++numWarningsPeer;
    if (removeSpeedReduction->isScheduled()) {
        cancelEvent(removeSpeedReduction);
    } else {
        vehicleController->setSpeedFactor(0.5);
    }
    scheduleAt(simTime() + warningDuration, removeSpeedReduction);
}
