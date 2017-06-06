#include "artery/veins/VeinsObstacleControl.h"
#include "traci/Core.h"
#include "traci/LiteAPI.h"
#include "traci/Position.h"

Define_Module(VeinsObstacleControl)

void VeinsObstacleControl::initialize(int stage)
{
    Veins::ObstacleControl::initialize(stage);
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
    auto boundary = traci.simulation().getNetBoundary();
    unsigned fetched = 0;
    for (const std::string& id : polygons.getIDList()) {
        std::string type = polygons.getType(id);
        if (this->isTypeSupported(type)) {
            std::vector<Coord> shape;
            for (const TraCIPosition& traci_point : polygons.getShape(id)) {
                using boost::units::si::meter;
                Position point = traci::position_cast(boundary, traci_point);
                shape.push_back(Coord { point.x / meter, point.y / meter});
            }
            this->addFromTypeAndShape(id, type, shape);
            ++fetched;
        }
    }
    EV_INFO << "Added " << fetched << " obstacles to " << this->getFullPath() << endl;
}
