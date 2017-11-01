package artery.inet.gemv2;

import inet.physicallayer.contract.packetlevel.IPathLoss;

module Gemv2 like IPathLoss
{
    parameters:
        @class(artery::gemv2::PathLoss);
        *.traciModule = default("traci");

    submodules:
        LOS: <default("TwoRayInterference")> like IPathLoss {
            @display("p=20,20");
            epsilon_r = default(1.003);
        }

        NLOSb: <default("LogNormalShadowing")> like IPathLoss {
            @display("p=20,40");
        }

        NLOSv: <default("LogNormalShadowing")> like IPathLoss {
            @display("p=20,60");
        }

        obstacles: ObstacleIndex {
            @display("p=50,20");
        }

        vehicles: VehicleIndex {
            @display("p=50,40");
        }

        classifier: LinkClassifier {
            @display("p=50,60");
            obstacleIndexModule = "^.obstacles";
            vehicleIndexModule = "^.vehicles";
        }
}
