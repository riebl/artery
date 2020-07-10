package artery.networking;

simple Router
{
    parameters:
        @class(Router);
        @signal[IdentityChanged](type=long);
        @signal[LinkReception](type=GeoNetPacket);

        @statistic[LinkLatency](source="messageAge(LinkReception)"; unit=s; record=vector?);

        string dccModule;
        string middlewareModule;
        string radioDriverModule;
        string runtimeModule;
        string securityModule;

        bool deferInitialBeacon = default(true);
        bool disableBeaconing = default(false);
        bool isMobile = default(true);

    gates:
        input radioDriverData;
        input radioDriverProperties;
}
