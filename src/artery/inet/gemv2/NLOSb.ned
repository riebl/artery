package artery.inet.gemv2;

import inet.physicallayer.contract.packetlevel.IPathLoss;

simple NLOSb like IPathLoss
{
    parameters:
        @display("i=block/control");
        @class(gemv2::NLOSb);
        string vehicleIndexModule;
        string obstacleIndexModule;
        string visualizerModule;

        double maxRange @unit(m);
        double pathLossExponent = default(2.9);
        double vehicleRelativePermittivity = default(6);
        double buildingRelativePermittivity = default(4.5);
        string polarization = default("vertical");
}
