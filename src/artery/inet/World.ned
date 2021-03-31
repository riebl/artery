package artery.inet;

import artery.StaticNodeManager;
import artery.storyboard.Storyboard;
import inet.environment.contract.IPhysicalEnvironment;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import traci.Manager;

network World
{
    parameters:
        bool withStoryboard = default(false);
        bool withPhysicalEnvironment = default(false);
        int numRoadSideUnits = default(0);
        traci.mapper.personType = default("artery.inet.Person");
        traci.mapper.vehicleType = default("artery.inet.Car");
        traci.nodes.personSinkModule = default(".mobility");
        traci.nodes.vehicleSinkModule = default(".mobility");
        storyboard.middlewareModule = default(".middleware");

        int numProbeCols = default(0);
        int numProbeRows = default(0);
        double probeInterval @unit(m) = default(25m);

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

        physicalEnvironment: <default("PhysicalEnvironment")> like IPhysicalEnvironment if withPhysicalEnvironment {
            parameters:
                @display("p=100,20");
        }

        storyboard: Storyboard if withStoryboard {
            parameters:
                @display("p=140,20");
        }

        rsu[numRoadSideUnits]: RSU {
            parameters:
                mobility.initFromDisplayString = false;
        }

        staticNodes: StaticNodeManager {
            parameters:
                @display("p=20,40");
                waitForTraCI = default(true);
        }

        probes[numProbeCols * numProbeRows]: PassiveProbe {
            parameters:
                mobility.numHosts = numProbeCols * numProbeRows;
                mobility.columns = numProbeCols;
                mobility.rows = numProbeRows;
        }
}
