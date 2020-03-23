package ots;

simple Core
{
    parameters:
        @signal[ots-gtu-add](type=string);
        @signal[ots-gtu-remove](type=string);
        @signal[ots-gtu-position](type=GtuObject);

        double stepLength @unit(second) = default(0.1s);
        string otsEndpoint = default("tcp://localhost:8888");
        string otsNetworkFile;
        int otsSeed = default(1);
        double otsWarmupDuration @unit(second) = default(0s);
        double otsRunDuration @unit(second);

        string simFederation = default("ots-demo");
        string simSender = default("Artery");
        string simReceiver = default("OTS");
}
