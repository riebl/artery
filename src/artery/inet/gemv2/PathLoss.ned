package artery.inet.gemv2;

import inet.physicallayer.contract.packetlevel.IPathLoss;

module Gemv2 like IPathLoss
{
    parameters:
        @class(artery::gemv2::PathLoss);
        *.traciModule = default("traci");
        bool withSmallScaleVariations = default(true);
        double rangeLOS @unit(m) = default(1000 m);
        double rangeNLOSv @unit(m) = default(400 m);
        double rangeNLOSb @unit(m) = default(300 m);

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

        smallScaleVariations: SmallScaleVariation if withSmallScaleVariations {
            @display("p=20,80");
            obstacleIndexModule = "^.obstacles";
            vehicleIndexModule = "^.vehicles";
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
