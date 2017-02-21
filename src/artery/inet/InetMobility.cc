#include "artery/inet/InetMobility.h"
#include "artery/traci/VehicleController.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/geometry/common/CanvasProjection.h>
#include <inet/visualizer/mobility/MobilityCanvasVisualizer.h>

Define_Module(InetMobility);

using namespace traci;

void InetMobility::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        m_visualRepresentation = inet::getModuleFromPar<cModule>(par("visualRepresentation"), this, false);
        m_antennaHeight = par("antennaHeight");
        WATCH(m_id);
        WATCH(m_position);
        WATCH(m_speed);
        WATCH(m_orientation);
    } else if (stage == inet::INITSTAGE_PHYSICAL_ENVIRONMENT_2) {
        if (m_visualRepresentation) {
            auto visualizationTarget = m_visualRepresentation->getParentModule();
            m_canvasProjection = inet::CanvasProjection::getCanvasProjection(visualizationTarget->getCanvas());
        }
        updateVisualRepresentation();
    }
}

void InetMobility::initializeVehicle(LiteAPI* api, const std::string& id, const TraCIBoundary& boundary)
{
    ASSERT(api);
    m_traci = api;
    m_id = id;
    m_boundary = boundary;
    m_controller.reset(new VehicleController(id, *api));
}

void InetMobility::updateVehicle(const TraCIPosition& traci_pos, TraCIAngle traci_heading, double traci_speed)
{
    using boost::units::si::meter;
    const auto opp_pos = position_cast(m_boundary, traci_pos);
    const auto opp_angle = angle_cast(traci_heading);
    const double rad = opp_angle.radian();
    const inet::Coord direction { cos(rad), -sin(rad) };
    m_position = inet::Coord { opp_pos.x / meter, opp_pos.y / meter, m_antennaHeight };
    m_speed = direction * traci_speed;
    m_orientation.alpha = -rad;

    emit(inet::IMobility::mobilityStateChangedSignal, this);
    updateVisualRepresentation();
}

VehicleController* InetMobility::getVehicleController()
{
    ASSERT(m_controller);
    return m_controller.get();
}

double InetMobility::getMaxSpeed() const
{
    return NaN;
}

inet::Coord InetMobility::getCurrentPosition()
{
    return m_position;
}

inet::Coord InetMobility::getCurrentSpeed()
{
    return m_speed;
}

inet::EulerAngles InetMobility::getCurrentAngularPosition()
{
    return m_orientation;
}

inet::EulerAngles InetMobility::getCurrentAngularSpeed()
{
    return inet::EulerAngles::ZERO;
}

inet::Coord InetMobility::getConstraintAreaMax() const
{
    return inet::Coord { m_boundary.xMax, m_boundary.yMax, m_boundary.zMax };
}

inet::Coord InetMobility::getConstraintAreaMin() const
{
    return inet::Coord { m_boundary.xMin, m_boundary.yMin, m_boundary.zMin };
}

void InetMobility::updateVisualRepresentation()
{
    if (hasGUI() && m_visualRepresentation) {
        using inet::visualizer::MobilityCanvasVisualizer;
        MobilityCanvasVisualizer::setPosition(m_visualRepresentation, m_canvasProjection->computeCanvasPoint(getCurrentPosition()));
    }
}
