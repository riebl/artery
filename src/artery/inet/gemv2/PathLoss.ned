package artery.inet.gemv2;

import inet.physicallayer.contract.packetlevel.IPathLoss;

module Gemv2 like IPathLoss
{
    parameters:
        @class(artery::gemv2::PathLoss);

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

}
