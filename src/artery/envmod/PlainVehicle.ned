package artery.envmod;

import artery.inet.PlainVehicle;
import artery.envmod.CarIdentityRegistrant;

module PlainVehicle extends artery.inet.PlainVehicle
{
    parameters:
        @display("i=blocki/process;is=vs");
        @labels(node);

    submodules:
        carIdentityRegistrant: CarIdentityRegistrant {
            parameters:
                @display("p=53,300");
        }
}
