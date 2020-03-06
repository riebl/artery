package artery.ots;

import inet.common.geometry.common.IGeographicCoordinateSystem;
import inet.environment.contract.IPhysicalEnvironment;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import ots.Connector;

network World
{
    parameters:
        bool withPhysicalEnvironment = default(false);
        int numRoadSideUnits = default(0);

    submodules:
        ots: Connector {
            parameters:
                @display("p=20,20");
                gtuController.creationPolicy.moduleType = default("artery.ots.GTU");
                gtuController.creationPolicy.gtuSinkPath = default(".mobility");
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

        coordinateSystem: <default("SimpleGeographicCoordinateSystem")> like IGeographicCoordinateSystem {
            parameters:
                @display("p=140,20");
        }

        rsu[numRoadSideUnits]: RSU {
            parameters:
                mobility.initFromDisplayString = false;
        }
}
