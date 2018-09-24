package artery.inet.gemv2;

import inet.physicallayer.pathloss.FreeSpacePathLoss;

module NLOSf extends FreeSpacePathLoss
{
    parameters:
        @display("i=block/control");
        @class(artery::gemv2::NLOSf);
        string foliageIndexModule;
        string visualizerModule;
}
