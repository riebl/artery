package artery.networking;

simple FsmDccEntity like IDccEntity
{
    parameters:
        @class(FsmDccEntity);
        string radioDriverModule;
        string routerModule;
        string runtimeModule;

        string ChannelProbeProcessor = default("SmoothingLocal");
        string StateMachine = default("FullyMeshed");
        string TransmitRateControl = default("Bursty");
        string NetworkEntity = default("");

        double targetCbr = default(0.59);
        int queueLength = default(2);

    gates:
        output radioDriverData;
}
