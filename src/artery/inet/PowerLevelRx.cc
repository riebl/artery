/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/inet/PowerLevelRx.h"
#include "artery/inet/VanetRadio.h"
#include "inet/common/INETMath.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/ieee80211/mac/contract/IContention.h"
#include "inet/physicallayer/analogmodel/packetlevel/ScalarNoise.h"
#include "inet/physicallayer/analogmodel/packetlevel/ScalarReception.h"
#include "inet/physicallayer/backgroundnoise/IsotropicScalarBackgroundNoise.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/physicallayer/contract/packetlevel/IRadioMedium.h"
#include <cmath>

using namespace omnetpp;
namespace phy = inet::physicallayer;

namespace artery
{

Define_Module(PowerLevelRx)

const simsignal_t PowerLevelRx::ChannelLoadSignal = cComponent::registerSignal("ChannelLoad");

PowerLevelRx::PowerLevelRx()
{
}

PowerLevelRx::~PowerLevelRx()
{
    cancelAndDelete(mChannelReportTrigger);
}

void PowerLevelRx::initialize(int stage)
{
    Rx::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        mCcaSignalThreshold = inet::mW { inet::math::dBm2mW(par("ccaSignalThreshold")) };
        mCcaNoiseThreshold = inet::mW { inet::math::dBm2mW(par("ccaNoiseThreshold")) };
        mCbrThreshold = inet::mW { inet::math::dBm2mW(par("cbrThreshold")) };
        mCbrWithTx = par("cbrWithTx");

        mChannelReportInterval = simtime_t { 100, SIMTIME_MS };
        mChannelReportTrigger = new cMessage("report CL");
        if (par("asyncChannelReport").boolValue()) {
            scheduleAt(simTime() + mChannelReportInterval, mChannelReportTrigger);
        } else {
            double cycle = simTime() / mChannelReportInterval;
            scheduleAt((1.0 + std::ceil(cycle)) * mChannelReportInterval, mChannelReportTrigger);
        }

        mChannelLoadSampler.reset();
        omnetpp::createWatch("channelLoadSampler", mChannelLoadSampler);
    } else if (stage == inet::INITSTAGE_PHYSICAL_LAYER) {
        mRadio = inet::getModuleFromPar<phy::IRadio>(par("radioModule"), this);
        auto radioMedium = mRadio->getMedium();
        mCommunicationCache = const_cast<phy::ICommunicationCache*>(radioMedium->getCommunicationCache());

        auto bgNoise = radioMedium->getBackgroundNoise();
        if (auto scalarBgNoise = dynamic_cast<const phy::IsotropicScalarBackgroundNoise*>(bgNoise)) {
            mBackgroundNoise = scalarBgNoise->getPower();
        } else {
            mBackgroundNoise = inet::W { 0 };
            if (bgNoise) {
                EV_ERROR << "assuming zero background noise (incompatible type at radio medium)" << endl;
            } else {
                EV_INFO << "assuming zero background noise (none configured at radio medium)" << endl;
            }
        }

        omnetpp::check_and_cast<omnetpp::cModule*>(mRadio)->subscribe(VanetRadio::RadioFrameSignal, this);
    }
}

void PowerLevelRx::handleMessage(cMessage* msg)
{
    if (msg == mChannelReportTrigger) {
        emit(ChannelLoadSignal, mChannelLoadSampler.cbr());
        scheduleAt(simTime() + mChannelReportInterval, mChannelReportTrigger);
    } else {
        Rx::handleMessage(msg);
    }
}

void PowerLevelRx::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal, omnetpp::cObject*, omnetpp::cObject*)
{
    Enter_Method_Silent();

    if (signal == VanetRadio::RadioFrameSignal) {
        recomputeMediumFree();
    }
}

void PowerLevelRx::recomputeMediumFree()
{
    const bool oldMediumFree = mediumFree;

    if (receptionState == phy::IRadio::ReceptionState::RECEPTION_STATE_RECEIVING) {
        const phy::ITransmission* transmission = mRadio->getReceptionInProgress();
        if (transmission) {
            const phy::IReception* reception = mRadio->getMedium()->getReception(mRadio, transmission);
            auto scalarReception = omnetpp::check_and_cast<const phy::ScalarReception*>(reception);
            const auto signalPower = scalarReception->getPower();

            const phy::INoise* noise = mRadio->getMedium()->getNoise(mRadio, transmission);
            auto scalarNoise = omnetpp::check_and_cast<const phy::ScalarNoise*>(noise);
            const auto noisePower = scalarNoise->computeMaxPower(reception->getStartTime(), reception->getEndTime());

            mediumFree = !endNavTimer->isScheduled() &&
                signalPower <= mCcaSignalThreshold &&
                signalPower + noisePower <= mCcaNoiseThreshold;
            mChannelLoadSampler.busy(signalPower + noisePower > mCbrThreshold);
        } else {
            error("no reception in progress though reception state is 'receiving'");
        }
    } else if (receptionState == phy::IRadio::ReceptionState::RECEPTION_STATE_BUSY) {
        static const auto busySymbol = omnetpp::SimTime { 8, omnetpp::SIMTIME_US };
        const auto busyStart = omnetpp::simTime();
        const auto busyEnd = busyStart + busySymbol;
        auto transmissions = mCommunicationCache->computeInterferingTransmissions(mRadio, busyStart, busyEnd);
        if (transmissions) {
            mediumFree = !endNavTimer->isScheduled();
            inet::W busyPower = mBackgroundNoise;
            for (const phy::ITransmission* transmission : *transmissions)
            {
                const phy::IReception* reception = mRadio->getMedium()->getReception(mRadio, transmission);
                if (reception->getEndTime() <= busyStart) {
                    // ignore just ending transmissions when switching from RECEIVING to BUSY
                    continue;
                }
                auto scalarReception = omnetpp::check_and_cast<const phy::ScalarReception*>(reception);
                mediumFree &= scalarReception->getPower() < mCcaSignalThreshold;
                busyPower += scalarReception->getPower();
            }

            mediumFree &= busyPower < mCcaNoiseThreshold;
            mChannelLoadSampler.busy(busyPower > mCbrThreshold);
        } else {
            error("no interfering transmissions found though reception state is 'busy'");
        }
        delete transmissions;
    } else if (transmissionState != phy::IRadio::TransmissionState::TRANSMISSION_STATE_UNDEFINED) {
        mediumFree = false;
        mChannelLoadSampler.busy(mCbrWithTx);
    } else {
        mediumFree = receptionState == phy::IRadio::ReceptionState::RECEPTION_STATE_IDLE && !endNavTimer->isScheduled();
        mChannelLoadSampler.busy(false);
    }

    if (mediumFree != oldMediumFree) {
        for (auto contention : contentions)
            contention->mediumStateChanged(mediumFree);
    }
}

} // namespace artery
