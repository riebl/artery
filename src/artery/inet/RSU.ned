package artery.inet;

import artery.application.StationaryMiddleware;
import artery.networking.Vanetza;
import artery.nic.IRadioDriver;
import inet.node.inet.INetworkNode;
import inet.linklayer.contract.IWirelessNic;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;

module RSU like INetworkNode
{
    parameters:
        @display("i=block/wrxtx;is=vs");
        @networkNode;
        @labels(node,ethernet-node,wireless-node);
        int numRadios = default(1);
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        mobility.visualRepresentation = "^";

    gates:
        input radioIn[numRadios] @directIn;
        inout pppg[] @labels(PPPFrame-conn);
        inout ethg[] @labels(EtherFrame-conn);

    submodules:
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=53,300;is=s");
        }

        wlan[numRadios]: <default("VanetNic")> like IWirelessNic {
            parameters:
                @display("p=216,406,row,60;q=queue");
        }

        radioDriver[numRadios]: RadioDriver {
            parameters:
                macModule = ".wlan[" + string(index) + "].mac";
                radioModule = ".wlan[" + string(index) + "].radio";
        }

        vanetza[numRadios]: Vanetza {
            parameters:
                *.middlewareModule = absPath("^.middleware");
                *.mobilityModule = absPath("^.mobility");
                *.radioDriverModule = absPath("^.radioDriver[" + string(index) + "]");
                *.traciCoreModule = "traci.core";
                position.typename = default("StationaryPositionProvider");
                runtime.datetime = middleware.datetime;
                router.isMobile = false;
        }

        mobility: <default("inet.mobility.static.StationaryMobility")> like IMobility {
            parameters:
                @display("p=53,200");
        }

        middleware: StationaryMiddleware {
            parameters:
                @display("p=50,100");
        }

    connections:
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> { @display("m=s"); } --> wlan[i].radioIn;

            wlan[i].upperLayerOut --> radioDriver[i].lowerLayerIn;
            wlan[i].upperLayerIn <-- radioDriver[i].lowerLayerOut;
        }

        radioDriver[0].upperLayer <--> vanetza.radioDriverData;
        radioDriver[0].properties --> vanetza.radioDriverProperties;
}
