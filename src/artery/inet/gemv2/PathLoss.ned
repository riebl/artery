package artery.inet.gemv2;

import inet.physicallayer.contract.packetlevel.IPathLoss;

module Gemv2 like IPathLoss
{
    parameters:
        @class(gemv2::PathLoss);
        *.traciModule = default("traci");
        bool withSmallScaleVariations = default(true);
        bool withVisualization = default(false);
        double rangeLOS @unit(m) = default(500 m);
        double rangeNLOSv @unit(m) = default(400 m);
        double rangeNLOSb @unit(m) = default(300 m);
        double rangeNLOSf @unit(m) = default(500 m);

        LOS.epsilon_r = default(1.003); // relative permittivity
        NLOSb.alpha = default(2.9); // path loss exponent
        NLOSb.sigma = default(0.0); // no flat fading
        NLOSb.maxRange = rangeNLOSb;
        **.polarization = default("vertical");

        **.vehicleIndexModule = absPath(".vehicles");
        **.obstacleIndexModule = absPath(".obstacles");
        **.foliageIndexModule = absPath(".foliage");
        **.visualizerModule = absPath(".visualizer");

    submodules:
        LOS: <default("TwoRayInterference")> like IPathLoss {
            @display("p=20,20");
        }

        NLOSb: <default("FreeSpacePathLoss")> like IPathLoss {
            @display("p=20,40");
        }

        NLOSf: <default("NLOSf")> like IPathLoss {
            @display("p=20,60");
        }

        NLOSv: <default("NLOSv")> like IPathLoss {
            @display("p=20,80");
        }

        smallScaleVariations: SmallScaleVariation if withSmallScaleVariations {
            @display("p=20,100");
        }

        obstacles: ObstacleIndex {
            @display("p=50,20");
        }

        foliage: FoliageIndex {
            @display("p=50,40");
        }

        vehicles: VehicleIndex {
            @display("p=50,60");
        }

        classifier: LinkClassifier {
            @display("p=50,80");
        }

        visualizer: Visualizer if withVisualization {
            @display("p=50,100");
        }
}
