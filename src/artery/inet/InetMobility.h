#ifndef INETMOBILITY_H_SKZPGILS
#define INETMOBILITY_H_SKZPGILS

#include "artery/traci/ControllableVehicle.h"
#include "traci/LiteAPI.h"
#include "traci/VehicleSink.h"
#include <inet/mobility/contract/IMobility.h>
#include <omnetpp/csimplemodule.h>
#include <memory>
#include <string>

namespace inet { class CanvasProjection; }


class InetMobility :
    public inet::IMobility, // for positions during INET transmissions
    public traci::VehicleSink, // for receiving updates from TraCI
    public ControllableVehicle, // for controlling the vehicle via TraCI
    public omnetpp::cSimpleModule // for... well... OMNeT++
{
public:
    // inet::IMobility interface
    double getMaxSpeed() const override;
    inet::Coord getCurrentPosition() override;
    inet::Coord getCurrentSpeed() override;
    inet::EulerAngles getCurrentAngularPosition() override;
    inet::EulerAngles getCurrentAngularSpeed() override;
    inet::Coord getConstraintAreaMax() const override;
    inet::Coord getConstraintAreaMin() const override;

    // traci::VehicleSink interface
    void initializeVehicle(traci::LiteAPI*, const std::string& id, const traci::TraCIBoundary&) override;
    void updateVehicle(const traci::TraCIPosition&, traci::TraCIAngle, double speed) override;

    // ControllableVehicle
    traci::VehicleController* getVehicleController() override;

protected:
    // cSimpleModule
    int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;

    virtual void updateVisualRepresentation();
    virtual const std::string& getVehicleId() { return m_id; }
    virtual traci::LiteAPI* getTraCI() { return m_traci; }

private:
    std::unique_ptr<traci::VehicleController> m_controller;
    std::string m_id;
    traci::LiteAPI* m_traci;
    traci::TraCIBoundary m_boundary;
    inet::Coord m_position;
    inet::Coord m_speed;
    inet::EulerAngles m_orientation;
    double m_antennaHeight;
    omnetpp::cModule* m_visualRepresentation;
    const inet::CanvasProjection *m_canvasProjection;
};

#endif /* INETMOBILITY_H_SKZPGILS */
