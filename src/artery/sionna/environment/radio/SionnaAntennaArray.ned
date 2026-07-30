package artery.sionna.environment.radio;

import inet.physicallayer.contract.packetlevel.IAntenna;

module SionnaAntennaArray like IAntenna
{
    parameters:
        @class(SionnaAntennaArray);
        @display("i=device/antennatower");

        int numAntennas = default(1);
        string mobilityModule = default(".mobility");
        double gain @unit(dB) = default(0dB);
        string sceneArrayRole @enum("", "tx", "rx") = default("");
}
