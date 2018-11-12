import inet.applications.contract.IUDPApp;

simple BlackIceReporter like IUDPApp
{
    parameters:
        int centralPort = default(9320);
        string centralAddress;
        string middlewareModule;

    gates:
        output udpOut;
        input udpIn;
}
