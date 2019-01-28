package artery.networking;

simple NoRateControlDccEntity like IDccEntity
{
    parameters:
        @class(NoRateControlDccEntity);
        string radioDriverModule;
        string routerModule;
        string runtimeModule;

        string ChannelProbeProcessor = default("SmoothingLocal");
        string NetworkEntity = default("");

        double targetCbr = default(0.59);
        int queueLength = default(2);

    gates:
        output radioDriverData;
}
