package artery.inet;

import inet.mobility.contract.IMobility;

module PlainPerson
{
    parameters:
        @display("i=abstract/person;is=vs");
        @labels(node);

        @statistic[posX](source="xCoord(mobilityPos(mobilityStateChanged))"; record=vector?);
        @statistic[posY](source="yCoord(mobilityPos(mobilityStateChanged))"; record=vector?);

        mobility.visualRepresentation = "^";

    submodules:
        mobility: <default("artery.inet.PersonMobility")> like IMobility {
            parameters:
                @display("p=50,200");
        }

}
