package artery.sionna.environment;

import inet.physicallayer.contract.packetlevel.IPathLoss;

//
// Ray-traced path loss model backed by the Sionna physical environment.
//
// This module is intended to sit under `radioMedium.pathLoss` and query the
// sibling `physicalEnvironment` module in the world.
//
module SionnaPathLoss like IPathLoss
{
    parameters:
        @display("i=block/wrxtx");
        @namespace(artery::sionna);
        @class(PathLoss);

        string physicalEnvironmentModule = default(absPath("^.^.physicalEnvironment"));

        double alpha = default(2);
        double systemLoss @unit(dB) = default(0dB);
        bool includeLineOfSight = default(true);
        bool includeReflections = default(true);
        bool includeDiffractions = default(false);
        int maxReflectionDepth = default(1);
        int maxDiffractionDepth = default(1);
        int maxNumPathsPerSrc = default(1000000);
        int samplesPerSrc = default(1000000);
        double maxRange @unit(m) = default(1000m);
}
