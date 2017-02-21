package artery.inet;

import artery.storyboard.Storyboard;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import traci.Manager;

network World
{
    parameters:
        bool withStoryboard = default(false);
        traci.mapper.vehicleType = default("artery.inet.Car");
        traci.nodes.vehicleSinkModule = default(".mobility");
        storyboard.middlewareModule = default(".middleware");

    submodules:
        traci: Manager {
            parameters:
                @display("p=20,20");
        }

        radioMedium: <default("Ieee80211ScalarRadioMedium")> like IRadioMedium {
            parameters:
                @display("p=60,20");
                mediumLimitCache.carrierFrequency = 5.9GHz;
        }

        storyboard: Storyboard if withStoryboard {
            parameters:
                @display("p=100,20");
        }
}
