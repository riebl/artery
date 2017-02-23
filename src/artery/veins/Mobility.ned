package artery.veins;

import org.car2x.veins.base.modules.BaseMobility;

simple Mobility extends BaseMobility like artery.traci.Mobility
{
    parameters:
        @class(VeinsMobility);
}
