package artery.inet;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import org.car2x.veins.base.modules.BaseWorldUtility;
import org.car2x.veins.modules.mobility.traci.ITraCIScenarioManager;

network World
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        @display("bgb=$playgroundSizeX,$playgroundSizeY");

    submodules:
        radioMedium: <default("Ieee80211ScalarRadioMedium")> like IRadioMedium {
            parameters:
                mediumLimitCache.carrierFrequency = 5.9GHz;
        }

        traciManager: <default("TraCIScenarioManagerArtery")> like ITraCIScenarioManager {
        }

        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=30,0;i=misc/globe");
        }
}
