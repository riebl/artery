package artery.inet;

import inet.physicallayer.contract.packetlevel.IPathLoss;

module DistanceSwitchPathLoss like IPathLoss
{
    parameters:
        @display("i=block/control");
        @class(artery::DistanceSwitchPathLoss);
        double thresholdDistance @unit(m);

    submodules:
        near: <> like IPathLoss {
        }

        far: <> like IPathLoss {
        }
}
