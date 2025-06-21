package artery.envmod;

import artery.inet.PlainVehicle;
import artery.envmod.IdentityRegistrant;

module PlainVehicle extends artery.inet.PlainVehicle
{
    submodules:
        carIdentityRegistrant: IdentityRegistrant {
            parameters:
                @display("p=53,300");
        }
}
