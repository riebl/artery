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
        string mobilityType = default("artery.veins.Mobility");

    gates:
        input radioIn @directIn; // gate for sendDirect

    submodules:
        appl: <applType> like IBaseApplLayer {
            parameters:
                @display("p=60,50");
                middleware.typename = "StationaryMiddleware";
                middleware.mobilityModule = ".mobility";
                middleware.radioDriverModule = ".appl.radioDriver";
                middleware.traciCoreModule = "traci.core";
        }

        nic: <nicType> like INic80211p {
            @display("p=60,150");
        }

        mobility: <mobilityType> like IMobility {
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
