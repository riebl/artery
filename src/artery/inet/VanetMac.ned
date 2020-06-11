package artery.inet;

import inet.linklayer.ieee80211.IIeee80211Mac;
import inet.linklayer.ieee80211.mac.MACProtocolBase;
import inet.linklayer.ieee80211.mac.contract.IRx;
import inet.linklayer.ieee80211.mac.contract.IStatistics;
import inet.linklayer.ieee80211.mac.contract.ITx;
import inet.linklayer.ieee80211.mac.coordinationfunction.Dcf;

// VanetMac is almost identical to INET's Ieee80211Mac except for the "hcf" submodule
module VanetMac extends MACProtocolBase like IIeee80211Mac
{
    parameters:
        string address = default("auto"); // MAC address as hex string (12 hex digits), or
                                          // "auto". "auto" values will be replaced by
                                          // a generated MAC address in init stage 0.
        string modeSet @enum("a", "b", "g(erp)", "g(mixed)", "n", "p") = default("g(mixed)");

        bool qosStation = default(false);

        *.rxModule = "^.rx";
        *.txModule = "^.tx";

        @display("i=block/layer");
        @class(::inet::ieee80211::Ieee80211Mac);
        @statistic[passedUpPk](title="packets passed to higher layer"; source=packetSentToUpper; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[sentDownPk](title="packets sent to lower layer"; source=packetSentToLower; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[rcvdPkFromHL](title="packets received from higher layer"; source=packetReceivedFromUpper; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[rcvdPkFromLL](title="packets received from lower layer"; source=packetReceivedFromLower; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);

        @signal[NF_MODESET_CHANGED](type=inet::physicallayer::Ieee80211ModeSet);
    submodules:
        dcf: Dcf {
            parameters:
                @display("p=100,100");
        }
        hcf: VanetHcf {
            parameters:
                @display("p=200,100");
        }
        rx: <default("Rx")> like IRx {
            parameters:
                @display("p=100,200");
        }
        tx: <default("Tx")> like ITx {
            parameters:
                @display("p=200,200");
        }
        statistics: <default("BasicStatistics")> like IStatistics {
            parameters:
                @display("p=300,100");
        }
}
