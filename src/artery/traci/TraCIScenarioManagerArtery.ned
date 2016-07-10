package artery.traci;

import org.car2x.veins.modules.mobility.traci.TraCIScenarioManagerLaunchd;

simple TraCIScenarioManagerArtery extends TraCIScenarioManagerLaunchd
{
    parameters:
        @class(TraCIScenarioManagerArtery);
        @signal[traci.add_node](type=string);
        @signal[traci.remove_node](type=string);
        @signal[traci.update_node](type=string);
        @signal[traci.update_step](type=simtime_t);
        string mobilityNodeModule = default(".mobility");
}
