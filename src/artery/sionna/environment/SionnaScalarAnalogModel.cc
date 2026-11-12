#include "SionnaScalarAnalogModel.h"

#include "PathLoss.h"

#include <inet/physicallayer/contract/packetlevel/IAntenna.h>
#include <inet/physicallayer/contract/packetlevel/IArrival.h>
#include <inet/physicallayer/contract/packetlevel/IRadio.h>
#include <inet/physicallayer/contract/packetlevel/IRadioMedium.h>
#include <inet/physicallayer/contract/packetlevel/IRadioSignal.h>
#include <inet/physicallayer/contract/packetlevel/ITransmission.h>
#include <omnetpp/cexception.h>

using namespace artery::sionna;

Define_Module(SionnaScalarAnalogModel);

// This override is needed since for some reason, even when known, receiver is not passed to PathLoss computation module.
// This override roughly does what ScalarAnalogModelBase::computeReceptionPower does, but also passes receiver to computePathLoss().
inet::W SionnaScalarAnalogModel::computeReceptionPower(
    const inet::physicallayer::IRadio* receiverRadio, const inet::physicallayer::ITransmission* transmission,
    const inet::physicallayer::IArrival* arrival) const
{
    const auto* radioMedium = receiverRadio->getMedium();
    const auto* pathLoss = dynamic_cast<const PathLoss*>(radioMedium->getPathLoss());
    if (pathLoss == nullptr) {
        throw omnetpp::cRuntimeError("SionnaScalarAnalogModel requires SionnaPathLoss");
    }

    const auto* transmitterRadio = transmission->getTransmitter();
    const auto* receiverAntenna = receiverRadio->getAntenna();
    const auto* transmitterAntenna = transmitterRadio->getAntenna();
    const auto* narrowbandSignalAnalogModel = check_and_cast<const inet::physicallayer::INarrowbandSignal*>(transmission->getAnalogModel());
    const auto* scalarSignalAnalogModel = check_and_cast<const inet::physicallayer::IScalarSignal*>(transmission->getAnalogModel());

    const inet::Coord receptionStartPosition = arrival->getStartPosition();
    const inet::EulerAngles transmissionDirection = computeTransmissionDirection(transmission, arrival);
    const inet::EulerAngles transmissionAntennaDirection = transmission->getStartOrientation() - transmissionDirection;
    const inet::EulerAngles receptionAntennaDirection = transmissionDirection - arrival->getStartOrientation();

    const double transmitterAntennaGain = transmitterAntenna->computeGain(transmissionAntennaDirection);
    const double receiverAntennaGain = receiverAntenna->computeGain(receptionAntennaDirection);
    const double sionnaPathGain = pathLoss->computePathLoss(receiverRadio, transmission, arrival);

    const inet::W transmissionPower = scalarSignalAnalogModel->getPower();
    return transmissionPower * std::min(1.0, transmitterAntennaGain * receiverAntennaGain * sionnaPathGain);
}
