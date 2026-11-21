#include "PathLoss.h"

#include <artery/sionna/environment/Compat.h>
#include <artery/sionna/environment/api/SionnaAPI.h>
#include <artery/sionna/environment/config/dynamic/TraciDynamicSceneConfigProvider.h>
#include <artery/sionna/environment/radio/SionnaVanetReceiver.h>
#include <artery/sionna/environment/radio/SionnaVanetTransmitter.h>
#include <inet/common/INETMath.h>
#include <inet/common/InitStages.h>
#include <inet/physicallayer/contract/packetlevel/IRadio.h>
#include <inet/physicallayer/contract/packetlevel/IRadioMedium.h>
#include <inet/physicallayer/contract/packetlevel/IRadioSignal.h>
#include <omnetpp/cexception.h>
#include <omnetpp/cmodule.h>

#include <algorithm>

using namespace artery::sionna;

Define_Module(PathLoss);

omnetpp::simsignal_t PathLoss::pathsSolved = omnetpp::cComponent::registerSignal("sionnaPathsSolved");

int PathLoss::numInitStages() const
{
    return inet::NUM_INIT_STAGES;
}

void PathLoss::initialize(int stage)
{
    FreeSpacePathLoss::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        solverParams_.includeLineOfSight = par("includeLineOfSight").boolValue();
        solverParams_.includeReflections = par("includeReflections").boolValue();
        solverParams_.includeDiffractions = par("includeDiffractions").boolValue();
        solverParams_.maxReflectionDepth = par("maxReflectionDepth").intValue();
        solverParams_.maxDiffractionDepth = par("maxDiffractionDepth").intValue();
        solverParams_.maxNumPathsPerSrc = par("maxNumPathsPerSrc").intValue();
        solverParams_.samplesPerSrc = par("samplesPerSrc").intValue();
        solverParams_.maxRange = par("maxRange").doubleValue();
        api_ = ISionnaAPI::get(getModuleByPath(par("physicalEnvironmentModule").stringValue()));
        subscribeToDynamicSceneUpdates(getSystemModule());
    } else if (stage == inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2) {
        solver_.emplace();
    }
}

void PathLoss::finish()
{
    unsubscribeFromDynamicSceneUpdates();
    invalidateCachedPaths();
    FreeSpacePathLoss::finish();
}

void PathLoss::onDynamicSceneEdited()
{
    invalidateCachedPaths();
}

std::ostream& PathLoss::printToStream(std::ostream& stream, int level) const
{
    stream << "SionnaPathLoss";
    if (level <= PRINT_LEVEL_DETAIL) {
        stream << ", maxRange = " << solverParams_.maxRange << ", los = " << solverParams_.includeLineOfSight
               << ", reflections = " << solverParams_.includeReflections << ", diffractions = " << solverParams_.includeDiffractions;
    }
    return stream;
}

const std::optional<py::Paths>& PathLoss::cachedPaths() const
{
    return paths_.object;
}

void PathLoss::invalidateCachedPaths() const
{
    paths_.object.reset();
    paths_.txIndices.clear();
    paths_.rxIndices.clear();
}

void PathLoss::solveCachedPaths() const
{
    if (paths_.object.has_value()) {
        return;
    }

    auto& scene = api_->scene();
    const auto maxDepth = std::max(solverParams_.maxDiffractionDepth, solverParams_.maxReflectionDepth);
    paths_.object = solver_->solve(
        scene, maxDepth, solverParams_.maxNumPathsPerSrc, solverParams_.samplesPerSrc, true, solverParams_.includeLineOfSight, solverParams_.includeReflections,
        false, false, solverParams_.includeDiffractions, false, true);

    paths_.txIndices.clear();
    paths_.rxIndices.clear();

    const auto txDevices = scene.orderedTransmitters();
    for (std::size_t i = 0; i < txDevices.size(); ++i) {
        paths_.txIndices.insert_or_assign(txDevices[i].first, i);
    }

    const auto rxDevices = scene.orderedReceivers();
    for (std::size_t i = 0; i < rxDevices.size(); ++i) {
        paths_.rxIndices.insert_or_assign(rxDevices[i].first, i);
    }

    // Const-ness has to be broken here since cache update is performed, which
    // already is non-const op.
    const_cast<PathLoss*>(this)->emit(pathsSolved, 1UL);
}

double PathLoss::computePathLoss(const inet::physicallayer::ITransmission* transmission, const inet::physicallayer::IArrival* arrival) const
{
    return NaN;
}

double PathLoss::computePathLoss(
    const inet::physicallayer::IRadio* receiverRadio, const inet::physicallayer::ITransmission* transmission,
    const inet::physicallayer::IArrival* arrival) const
{
    const inet::m distance(arrival->getStartPosition().distance(transmission->getStartPosition()));

    // Quick fail in case receive is unluckily.
    if (distance.get() > solverParams_.maxRange) {
        return 0.0;
    }

    auto scalarSignal = check_and_cast<const inet::physicallayer::IScalarSignal*>(transmission->getAnalogModel());

    // To get actual loss from paths, we need that both receiver and transmitter were
    // present in the scene (so paths would be calculated for them).

    auto* tx = dynamic_cast<const SionnaVanetTransmitter*>(transmission->getTransmitter()->getTransmitter());
    if (tx == nullptr) {
        auto* module = dynamic_cast<const omnetpp::cModule*>(transmission->getTransmitter());
        throw omnetpp::cRuntimeError("Radio %s has no Sionna transmitter", module ? module->getFullPath().c_str() : "<unknown>");
    }

    auto* rx = dynamic_cast<const SionnaVanetReceiver*>(receiverRadio->getReceiver());
    if (rx == nullptr) {
        auto* module = dynamic_cast<const omnetpp::cModule*>(receiverRadio);
        throw omnetpp::cRuntimeError("Radio %s has no Sionna receiver", module ? module->getFullPath().c_str() : "<unknown>");
    }

    solveCachedPaths();

    double gain;
    if (auto txIndex = paths_.txIndices.find(tx->sceneID()); txIndex == paths_.txIndices.end()) {
        throw omnetpp::cRuntimeError("Solved paths are missing transmitter index for %s", tx->sceneID().c_str());
    } else if (auto rxIndex = paths_.rxIndices.find(rx->sceneID()); rxIndex == paths_.rxIndices.end()) {
        throw omnetpp::cRuntimeError("Solved paths are missing receiver index for %s", rx->sceneID().c_str());
    } else {
        gain = paths_.object->pathGain(rxIndex->second, txIndex->second);
    }

    return std::clamp(gain, 0.0, 1.0);
}
