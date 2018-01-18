package artery.lte;

import inet.applications.contract.ITCPApp;
import inet.applications.contract.IUDPApp;
import inet.networklayer.configurator.ipv4.HostAutoConfigurator;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;
import inet.transportlayer.tcp.TCP;
import inet.transportlayer.udp.UDP;
import lte.stack.phy.ILteNic;
import lte.stack.phy.LteNicUe;
import lte.stack.phy.LteNicUeD2D;

module Car extends artery.inet.Car
{
    parameters:
        // INET network layer
        string networkLayerType = default("IPv4NetworkLayer");
        string routingTableType = default("IPv4RoutingTable");
        *.routingTableModule = default(absPath(".routingTable"));

        // LTE NIC
        bool d2dCapable = default(false);
        string nicType = default(d2dCapable ? "LteNicUeD2D" : "LteNicUe");
        int macNodeId = default(0);
        int masterId;

        // application
        int numUdpApps = default(0);
        int numTcpApps = default(0);

    gates:
        input lteRadioIn @directIn;

    submodules:
        configurator: HostAutoConfigurator {
            @display("p=50,25");
        }

        routingTable: <routingTableType> like IRoutingTable {
            @display("p=50,125");
        }

        networkLayer: <networkLayerType> like INetworkLayer {
            @display("p=150,125");
        }

        lteNic: <nicType> like ILteNic {
            @display("p=150,400");
            nodeType = "UE";
            d2dCapable = d2dCapable;
        }

        udpApp[numUdpApps]: <> like IUDPApp {
             @display("p=250,25,col,-100");
        }

        udp: UDP if numUdpApps > 0 {
            @display("p=250,125");
        }

        tcpApp[numTcpApps]: <> like ITCPApp {
             @display("p=350,25,col,-100");
        }

        tcp: TCP if numTcpApps > 0 {
            @display("p=350,125");
        }

    connections allowunconnected:
        lteNic.radioIn <-- lteRadioIn;

        networkLayer.ifOut++ --> lteNic.upperLayerIn;
        networkLayer.ifIn++ <-- lteNic.upperLayerOut;

        for i = 0..numUdpApps-1 {
            udpApp[i].udpOut --> udp.appIn++;
            udpApp[i].udpIn <-- udp.appOut++;
        }

        udp.ipOut --> networkLayer.transportIn++ if numUdpApps > 0;
        udp.ipIn <-- networkLayer.transportOut++ if numUdpApps > 0;

        for i = 0..numTcpApps-1 {
            tcpApp[i].tcpOut --> tcp.appIn++;
            tcpApp[i].tcpIn <-- tcp.appOut++;
        }

        tcp.ipOut --> networkLayer.transportIn++ if numTcpApps > 0;
        tcp.ipIn <-- networkLayer.transportout++ if numTcpApps > 0;
}
