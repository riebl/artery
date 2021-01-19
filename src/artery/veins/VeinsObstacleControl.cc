#include "artery/veins/VeinsObstacleControl.h"
#include "artery/traci/Cast.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/Position.h"
#include <boost/units/systems/si/length.hpp>

namespace artery
{

Define_Module(VeinsObstacleControl)

void VeinsObstacleControl::initialize(int stage)
{
    veins::ObstacleControl::initialize(stage);
    if (stage == 0) {
        subscribeTraCI(getSystemModule());
    }
}

void VeinsObstacleControl::traciInit()
{
    const char* traciCoreModule = par("traciCoreModule").stringValue();
    auto core = dynamic_cast<traci::Core*>(getModuleByPath(traciCoreModule));
    if (!core) {
        throw omnetpp::cRuntimeError("No traci.Core module found at %s", traciCoreModule);
    } else {
        fetchObstacles(core->getLiteAPI());
    }
}

void VeinsObstacleControl::fetchObstacles(traci::LiteAPI& traci)
{
    auto& polygons = traci.polygon();
    const traci::Boundary boundary { traci.simulation().getNetBoundary() };
    unsigned fetched = 0;
    for (const std::string& id : polygons.getIDList()) {
        std::string type = polygons.getType(id);
        if (this->isTypeSupported(type)) {
            std::vector<veins::Coord> shape;
            for (const traci::TraCIPosition& traci_point : polygons.getShape(id)) {
                using boost::units::si::meter;
                Position point = traci::position_cast(boundary, traci_point);
                shape.push_back(veins::Coord { point.x / meter, point.y / meter});
            }
            this->addFromTypeAndShape(id, type, shape);
            ++fetched;
        }
    }
    EV_INFO << "Added " << fetched << " obstacles to " << this->getFullPath() << endl;
}

} // namespace artery
