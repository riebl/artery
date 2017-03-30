package traci;

simple Core
{
    parameters:
        @class(traci::Core);
        @signal[traci.init](type=simtime_t);
        @signal[traci.step](type=simtime_t);
        @signal[traci.close](type=simtime_t);

        string launcherModule = default(".launcher");
        int version = default(0);
        bool selfStopping = default(true);
        double startTime @unit(second) = default(0.0s);
}
