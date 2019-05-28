package artery.veins;

import org.car2x.veins.base.modules.IBaseApplLayer;
import org.car2x.veins.base.modules.IMobility;
import org.car2x.veins.modules.nic.INic80211p;

module Car
{
    parameters:
        @networkNode;
        @display("i=block/wrxtx;is=vs");
        string applType = default("artery.veins.ApplicationLayer"); // type of application layer
        string nicType = default("org.car2x.veins.modules.nic.Nic80211p"); // type of network interface card
        string mobilityType = default("artery.veins.Mobility");

    gates:
        input radioIn; // gate for sendDirect

    submodules:
        appl: <applType> like IBaseApplLayer {
            parameters:
                @display("p=60,50");
                **.mobilityModule = absPath("^.mobility");
        }

        nic: <nicType> like INic80211p {
            @display("p=60,150");
            mac1609_4.useServiceChannel = default(false);
        }

        mobility: <mobilityType> like IMobility {
            parameters:
                @display("p=130,150");
                x = 0.0; // overwritten by TraCI
                y = 0.0; // overwritten by TraCI
                z = default(1.5);
        }

    connections:
        nic.upperLayerOut --> appl.lowerLayerIn;
        nic.upperLayerIn <-- appl.lowerLayerOut;
        nic.upperControlOut --> appl.lowerControlIn;
        nic.upperControlIn <-- appl.lowerControlOut;

        radioIn --> nic.radioIn;
}
