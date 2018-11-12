package artery.inet;

import inet.mobility.contract.IMobility;

simple AntennaMobility like IMobility
{
    parameters:
        @class(AntennaMobility);
        @signal[mobilityStateChanged];

        string mobilityModule;

        double offsetX @unit(m) = default(0 m);
        double offsetY @unit(m) = default(0 m);
        double offsetZ @unit(m) = default(0 m);

        double offsetAlpha @unit(rad) = default(0 rad);
        double offsetBeta @unit(rad) = default(0 rad);
        double offsetGamma @unit(rad) = default(0 rad);
}
