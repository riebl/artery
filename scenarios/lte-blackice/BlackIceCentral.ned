import inet.applications.contract.IUDPApp;

simple BlackIceCentral like IUDPApp
{
    parameters:
        int reportPort = default(9320);
        int queryPort = default(9321);

    gates:
        output udpOut;
        input udpIn;
}
