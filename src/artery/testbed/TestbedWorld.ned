package artery.testbed;

import artery.inet.World;

network TestbedWorld extends World
{
    submodules:
        otaInterface: <default("OtaInterfaceStub")> like OtaInterface {
            parameters:
                @display("p=70,40");
        }
}
