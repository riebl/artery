package artery.envmod;

import inet.mobility.contract.IMobility;
import artery.envmod.CarIdentityRegistrant;

module PlainVehicle
{
    parameters:
        @display("i=blocki/process;is=vs");
        @labels(node);
        mobility.visualRepresentation = "^";

    submodules:
        mobility: <default("artery.inet.Mobility")> like IMobility {
            parameters:
                @display("p=50,200");
        }

        carIdentityRegistrant: CarIdentityRegistrant {
            parameters:
                @display("p=53,300");
        }

}
