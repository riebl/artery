#include "BlackIceReporter.h"
#include "lte_msgs/BlackIceWarning_m.h"
#include "artery/application/Middleware.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/traci/VehicleController.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <omnetpp/checkandcast.h>

using namespace omnetpp;

Define_Module(BlackIceReporter)

static const simsignal_t storyboardSignal = cComponent::registerSignal("StoryboardSignal");

void BlackIceReporter::initialize(int stage)
{
    if (stage == artery::InitStages::Prepare) {
        tractionLosses = 0;
        WATCH(tractionLosses);
    } else if (stage == artery::InitStages::Self) {
        socket.setOutputGate(gate("udpOut"));
        auto centralAddress = inet::L3AddressResolver().resolve(par("centralAddress"));
        socket.connect(centralAddress, par("centralPort"));

        auto mw = inet::getModuleFromPar<artery::Middleware>(par("middlewareModule"), this);
        mw->subscribe(storyboardSignal, this);
        vehicleController = artery::notNullPtr(mw->getFacilities().get_const_ptr<traci::VehicleController>());
    }
}

int BlackIceReporter::numInitStages() const
{
    return artery::InitStages::Total;
}

void BlackIceReporter::finish()
{
    socket.close();
    recordScalar("traction losses", tractionLosses);
}

void BlackIceReporter::receiveSignal(cComponent*, simsignal_t sig, cObject* obj, cObject*)
{
    if (sig == storyboardSignal) {
        auto sigobj = check_and_cast<artery::StoryboardSignal*>(obj);
        if (sigobj->getCause() == "traction loss") {
            ++tractionLosses;
            sendReport();
        }
    }
}

void BlackIceReporter::sendReport()
{
    Enter_Method_Silent();
    using boost::units::si::meter;
    using boost::units::si::meter_per_second;
    auto report = new BlackIceReport("reporting black ice");
    report->setPositionX(vehicleController->getPosition().x / meter);
    report->setPositionY(vehicleController->getPosition().y / meter);
    report->setSpeed(vehicleController->getSpeed() / meter_per_second);
    report->setTime(simTime());
    socket.send(report);
}
