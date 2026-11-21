#pragma once

#include "SionnaActorBase.h"

#include <artery/inet/InetMobility.h>

namespace artery::sionna
{

class SionnaVehicleActor : public artery::InetVehicleMobility, public SionnaActorBase
{
public:
    // omnetpp::cSimpleModule implementation.
    void initialize(int stage) override;
    void finish() override;

    // ISionnaActor implementation.
    const std::string& sumoId() const override;

    // traci::VehicleSink implementation.
    void initializeVehicle(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed) override;
    void updateVehicle(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed) override;
};

}  // namespace artery::sionna
