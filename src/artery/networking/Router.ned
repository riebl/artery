package artery.networking;

simple Router
{
    parameters:
        @class(Router);
        string dccModule;
        string middlewareModule;
        string radioDriverModule;
        string runtimeModule;
        string securityModule;

        bool deferInitialBeacon = default(true);
        bool isMobile = default(true);

    gates:
        input radioDriverData;
        input radioDriverProperties;
}
