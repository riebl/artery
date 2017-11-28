package artery.inet.gemv2;

import inet.physicallayer.contract.packetlevel.IPathLoss;

simple NLOSv like IPathLoss
{
    parameters:
        @class(artery::gemv2::NLOSv);
        string vehicleIndexModule;
}
