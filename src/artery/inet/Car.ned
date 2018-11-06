package artery.inet;

import artery.application.VehicleMiddleware;
import artery.networking.Vanetza;
import artery.nic.IRadioDriver;
import inet.node.inet.INetworkNode;
import inet.linklayer.contract.IWirelessNic;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;

// NOTE:
// Car does not inherit from PlainVehicle on purpose to avoid a "use-after-free" bug.
// See https://github.com/riebl/artery/pull/27 for details.

module Car like INetworkNode
{
    parameters:
        @display("i=block/wrxtx;is=vs");
        @networkNode;
        @labels(node,ethernet-node,wireless-node);

        @statistic[posX](source="xCoord(mobilityPos(mobilityStateChanged))"; record=vector?);
        @statistic[posY](source="yCoord(mobilityPos(mobilityStateChanged))"; record=vector?);

        int numRadios = default(1);
        bool withAntennaMobility = default(false);
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        mobility.visualRepresentation = "^";

    gates:
        input radioIn[numRadios] @directIn;
        inout pppg[] @labels(PPPFrame-conn);
        inout ethg[] @labels(EtherFrame-conn);

    submodules:
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=50,300");
        }

        wlan[numRadios]: <default("VanetNic")> like IWirelessNic {
            parameters:
                @display("p=250,400,row,100;q=queue");
                radio.antenna.mobilityModule = absPath(withAntennaMobility ? "^.antennaMobility" : "^.mobility");
        }

        antennaMobility: AntennaMobility if withAntennaMobility == true {
            parameters:
                @display("p=100,200");
                mobilityModule = "^.mobility";
        }

        radioDriver[numRadios]: RadioDriver {
            parameters:
                @display("p=250,300,row,100");
                macModule = ".wlan[" + string(index) + "].mac";
                radioModule = ".wlan[" + string(index) + "].radio";
        }

        vanetza[numRadios]: Vanetza {
            parameters:
                @display("p=250,200,row,100");
                *.middlewareModule = absPath("^.middleware");
                *.mobilityModule = absPath("^.mobility");
                *.radioDriverModule = absPath("^.radioDriver[" + string(index) + "]");
                runtime.datetime = middleware.datetime;
        }

        mobility: <default("artery.inet.Mobility")> like IMobility {
            parameters:
                @display("p=50,200");
        }

        middleware: VehicleMiddleware {
            parameters:
                @display("p=250,100");
                mobilityModule = ".mobility";
        }

    connections:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;

            wlan[i].upperLayerOut --> radioDriver[i].lowerLayerIn;
            wlan[i].upperLayerIn <-- radioDriver[i].lowerLayerOut;

            radioDriver[i].upperLayer <--> vanetza[i].radioDriverData;
            radioDriver[i].properties --> vanetza[i].radioDriverProperties;
        }
}
