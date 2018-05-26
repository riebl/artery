package artery.inet;

import inet.mobility.contract.IMobility;

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
}
