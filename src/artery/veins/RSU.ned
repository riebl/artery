package artery.veins;

import org.car2x.veins.base.modules.IBaseApplLayer;
import org.car2x.veins.base.modules.IMobility;
import org.car2x.veins.modules.nic.INic80211p;

module RSU
{
    parameters:
        @networkNode;
        @display("i=block/wrxtx;is=vs");
        string applType = default("artery.veins.ApplicationLayer"); // type of application layer
        string nicType = default("org.car2x.veins.modules.nic.Nic80211p"); // type of network interface card

    gates:
        input radioIn @directIn; // gate for sendDirect

    submodules:
        appl: <applType> like IBaseApplLayer {
            parameters:
                @display("p=60,50");
                middleware.typename = "StationaryMiddleware";
                vanetza.position.typename = "StationaryPositionProvider";
                vanetza.router.isMobile = false;
                **.mobilityModule = absPath("^.mobility");
                **.traciCoreModule = "traci.core";
        }

        nic: <nicType> like INic80211p {
            @display("p=60,150");
            mac1609_4.useAcks = default(true);
            mac1609_4.useServiceChannel = default(false);
            phy80211p.minPowerLevel = default(-110dBm);
            phy80211p.noiseFloor = default(-98dBm);
            phy80211p.useNoiseFloor = default(true);
            phy80211p.usePropagationDelay = default(true);
        }

        mobility: org.car2x.veins.base.modules.BaseMobility {
            parameters:
                @display("p=130,150");
        }

    connections:
        nic.upperLayerOut --> appl.lowerLayerIn;
        nic.upperLayerIn <-- appl.lowerLayerOut;
        nic.upperControlOut --> appl.lowerControlIn;
        nic.upperControlIn <-- appl.lowerControlOut;

        radioIn --> nic.radioIn;
}
