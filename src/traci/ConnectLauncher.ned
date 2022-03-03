package traci;

simple ConnectLauncher like Launcher
{
    parameters:
        @class(traci::ConnectLauncher);
        string hostname = default("localhost");
        int port;
        int clientId = default(1);
}
