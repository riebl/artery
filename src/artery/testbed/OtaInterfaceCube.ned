package artery.testbed;

import artery.testbed.OtaInterface;

simple OtaInterfaceCube like OtaInterface
{
    parameters:
        @class(OtaInterfaceCube);
        string cubeAddress;
        int cubeTxPort = default(33210);
        int cubeRxPort = default(33211);
}
