package artery.inet;

import inet.mobility.contract.IMobility;

simple Mobility like IMobility, artery.traci.Mobility
{
    parameters:
        @signal[mobilityStateChanged];
        string visualRepresentation = default("");
        double antennaHeight @unit(m) = default(1.5m);
}

simple VehicleMobility extends Mobility
{
    parameters:
        @class(InetVehicleMobility);
}

simple PersonMobility extends Mobility
{
    parameters:
        @class(InetPersonMobility);
}
