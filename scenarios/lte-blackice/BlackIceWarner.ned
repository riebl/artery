import inet.applications.contract.IUDPApp;

simple BlackIceWarner like IUDPApp
{
    parameters:
        int centralPort = default(9321);
        string centralAddress;
        string middlewareModule;
        double pollingRadius @unit(m) = default(100 m);
        double pollingInterval @unit(s) = default(1.0 s);

    gates:
        output udpOut;
        input udpIn;
}
