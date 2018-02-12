package artery.inet.gemv2;

import inet.physicallayer.contract.packetlevel.IPathLoss;

simple NLOSb like IPathLoss
{
    parameters:
        @class(artery::gemv2::NLOSb);
        string vehicleIndexModule;
        string obstacleIndexModule;
        
        double maxRange @unit(m) = default(500 m);
        double veh = default(0.3);
        double build = default(0.3);
}
