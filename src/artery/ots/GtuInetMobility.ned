package artery.ots;

import inet.mobility.contract.IMobility;

simple GtuInetMobility like IMobility
{
    parameters:
        @signal[mobilityStateChanged];
        @signal[gtuPositionChanged](type=ots::GtuObject);
        string visualRepresentation = default("");
        string coordinateSystemModule = default("coordinateSystem");
}
