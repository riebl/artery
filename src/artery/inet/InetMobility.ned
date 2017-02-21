package artery.inet;

import inet.mobility.contract.IMobility;

simple Mobility like IMobility
{
    parameters:
        @class(InetMobility);
        @signal[mobilityStateChanged];
        string visualRepresentation = default("");
        double antennaHeight @unit(m) = default(1.5m);
}
