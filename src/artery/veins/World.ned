package artery.veins;

import artery.storyboard.Storyboard;
import artery.veins.ObstacleControl;
import artery.veins.ConnectionManager;
import artery.veins.RSU;
import org.car2x.veins.base.modules.BaseWorldUtility;
import traci.Manager;

network World
{
    parameters:
        bool withObstacles = default(true);
        bool withStoryboard = default(false);
        int numRoadSideUnits = default(0);

        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        @display("bgb=$playgroundSizeX,$playgroundSizeY");

    submodules:
        connectionManager: ConnectionManager {
            @display("p=40,100");
        }

        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=40,150");
        }

        obstacles: ObstacleControl if withObstacles {
            parameters:
                traciCoreModule = "^.traci.core";
                @display("p=40,200");
        }

        traci: Manager {
            parameters:
                mapper.vehicleType = default("artery.veins.Car");
                nodes.vehicleSinkModule = default(".mobility");
                // persons are not (yet) supported with Veins-based setup
                mapper.personType = default("");
                nodes.personSinkModule = default("");
                @display("p=20,20");
        }

        storyboard: Storyboard if withStoryboard {
            parameters:
                middlewareModule = default(".appl.middleware");
                @display("p=100,20");
        }

        rsu[numRoadSideUnits]: RSU {
        }
}
