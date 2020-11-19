/*
* Artery V2X Simulation Framework
* Copyright 2019 Raphael Riebl et al.
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#include "artery/envmod/sensor/FovSensor.h"
#include "artery/envmod/service/CollectivePerceptionMockMessage.h"
#include "artery/envmod/service/CollectivePerceptionMockService.h"
#include "artery/utility/InitStages.h"
#include "artery/utility/PointerCheck.h"
#include <omnetpp/checkandcast.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cpacket.h>

namespace artery
{

Define_Module(CollectivePerceptionMockService)

namespace
{

omnetpp::simsignal_t camSentSignal = omnetpp::cComponent::registerSignal("CamSent");
omnetpp::simsignal_t cpmSentSignal = omnetpp::cComponent::registerSignal("CpmSent");
omnetpp::simsignal_t cpmReceivedSignal = omnetpp::cComponent::registerSignal("CpmReceived");

} // namespace


CollectivePerceptionMockService::~CollectivePerceptionMockService()
{
    cancelAndDelete(mTrigger);
}

int CollectivePerceptionMockService::numInitStages() const
{
    return InitStages::Total;
}

void CollectivePerceptionMockService::initialize(int stage)
{
    if (stage == InitStages::Prepare) {
        ItsG5Service::initialize();
        mPositionProvider = &getFacilities().get_const<PositionProvider>();
        mEnvironmentModel = &getFacilities().get_const<LocalEnvironmentModel>();

        mDccProfile = par("dccProfile");
        mLengthHeader = par("lengthHeader");
        mLengthFovContainer = par("lengthFovContainer");
        mLengthObjectContainer = par("lengthObjectContainer");

        mGenerateAfterCam = par("generateAfterCam");
        mCpmOffset = par("cpmOffset");
        mCpmInterval = par("cpmInterval");
    } else if (stage == InitStages::Self) {
        mTrigger = new omnetpp::cMessage("triggger mock-up CPM");
        if (mGenerateAfterCam) {
            subscribe(camSentSignal);
        }
        mHostId = getFacilities().get_const<Identity>().host->getId();
    } else if (stage == InitStages::Propagate) {
        for (const Sensor* sensor : mEnvironmentModel->getSensors()) {
            // consider only sensors with a field of view
            if (auto fovSensor = dynamic_cast<const FovSensor*>(sensor)) {
                CollectivePerceptionMockMessage::FovContainer fovContainer;
                fovContainer.sensorId = sensor->getId();
                fovContainer.position = sensor->position();
                fovContainer.fov = fovSensor->getFieldOfView();
                mFovContainers.emplace_back(std::move(fovContainer));
                mSensors.insert(sensor);
            }
        }
    }
}

void CollectivePerceptionMockService::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == mTrigger) {
        generatePacket();
    } else {
        ItsG5Service::handleMessage(msg);
    }
}

void CollectivePerceptionMockService::receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t signal,
        omnetpp::cObject* obj, omnetpp::cObject*)
{
    if (signal == camSentSignal) {
        scheduleAt(omnetpp::simTime() + mCpmOffset, mTrigger);
    }
}

void CollectivePerceptionMockService::trigger()
{
    if (!mGenerateAfterCam && !mTrigger->isScheduled()) {
        auto channel = getFacilities().get_const<MultiChannelPolicy>().primaryChannel(vanetza::aid::CP);
        auto netifc = notNullPtr(getFacilities().get_const<NetworkInterfaceTable>().select(channel));
        vanetza::dcc::TransmitRateThrottle* trc = notNullPtr(netifc->getDccEntity().getTransmitRateThrottle());
        vanetza::dcc::TransmissionLite tx { static_cast<vanetza::dcc::Profile>(mDccProfile), 0 };
        const omnetpp::SimTime interval = std::chrono::duration<double>(trc->interval(tx)).count();
        const omnetpp::SimTime next = omnetpp::simTime() + mCpmOffset;
        if (mTrigger->getArrivalTime() + std::max(interval, mCpmInterval) <= next) {
            scheduleAt(next, mTrigger);
        }
    }
}

void CollectivePerceptionMockService::indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket* packet)
{
    auto cpm = omnetpp::check_and_cast<CollectivePerceptionMockMessage*>(packet);
    emit(cpmReceivedSignal, cpm);
    delete packet;
}

void CollectivePerceptionMockService::generatePacket()
{
    using namespace vanetza;
    btp::DataRequestB req;
    req.destination_port = host_cast<PortNumber>(getPortNumber());
    req.gn.transport_type = geonet::TransportType::SHB;
    req.gn.traffic_class.tc_id(mDccProfile);
    req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

    auto packet = new CollectivePerceptionMockMessage();
    packet->setByteLength(mLengthHeader);

    if (mFovLast + mFovInterval <= omnetpp::simTime()) {
        packet->setFovContainers(mFovContainers);
        packet->addByteLength(mLengthFovContainer * mFovContainers.size());
        mFovLast = omnetpp::simTime();
    }

    std::vector<CollectivePerceptionMockMessage::ObjectContainer> objectContainers;
    using TrackedObject = LocalEnvironmentModel::TrackedObject;
    for (const TrackedObject& object : mEnvironmentModel->allObjects()) {
        const LocalEnvironmentModel::Tracking& tracking = object.second;
        if (tracking.expired()) {
            // skip objects with lost tracking
            continue;
        }

        for (const auto& sensor : tracking.sensors()) {
            if (mSensors.find(sensor.first) != mSensors.end()) {
                CollectivePerceptionMockMessage::ObjectContainer objectContainer;
                objectContainer.object = object.first;
                objectContainer.objectId = tracking.id();
                objectContainer.sensorId = sensor.first->getId();
                objectContainer.timeOfMeasurement = sensor.second.last();
                objectContainers.emplace_back(std::move(objectContainer));
            }
        }
    }
    packet->addByteLength(mLengthObjectContainer * objectContainers.size());
    packet->setObjectContainers(std::move(objectContainers));
    packet->setSourceStation(mHostId);

    emit(cpmSentSignal, packet);
    request(req, packet);
}

} // namespace artery
