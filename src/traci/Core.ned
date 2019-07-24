package traci;

simple Core
{
    parameters:
        @class(traci::Core);
        @signal[traci.init](type=simtime_t);
        @signal[traci.step](type=simtime_t);
        @signal[traci.close](type=simtime_t);

        string launcherModule = default(".launcher");
        string subscriptionsModule = default(".subscriptions");

        // check API version of TraCI server:
        //  -1 ignores reported version entirely
        //   0 matches API version bundled with Artery's TraCI client code
        //   positive integers match the given TraCI API version (e.g. SUMO 1.1.0 uses API version 19)
        int version = default(-1);
        bool selfStopping = default(true);
        double startTime @unit(second) = default(0.0s);
}
