#pragma once

#include "SionnaActorBase.h"

#include <artery/inet/InetMobility.h>

namespace artery::sionna
{

class SionnaPersonActor : public artery::InetPersonMobility, public SionnaActorBase
{
public:
    // omnetpp::cSimpleModule implementation.
    void initialize(int stage) override;
    void finish() override;

    // ISionnaActor implementation.
    const std::string& sumoId() const override;

    // traci::PersonSink implementation.
    void initializePerson(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed) override;
    void updatePerson(const traci::TraCIPosition& position, traci::TraCIAngle heading, double speed) override;
};

}  // namespace artery::sionna
