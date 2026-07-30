package artery.sionna.environment.radio;

import artery.sionna.environment.radio.SionnaAntennaArray;

module SionnaPlanarAntennaArray extends SionnaAntennaArray
{
    parameters:
        @class(SionnaPlanarAntennaArray);

        int numRows = default(1);
        int numCols = default(1);
        string pattern = default("iso");

        double verticalSpacing = default(0.5);
        double horizontalSpacing = default(0.5);
        string polarization = default("V");
        string polarizationModel = default("tr38901_2");
}
