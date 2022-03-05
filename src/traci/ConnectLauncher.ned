package traci;

simple ConnectLauncher like Launcher
{
    parameters:
        @class(traci::ConnectLauncher);
        string hostname = default("localhost");
        int port;

        // Every TraCI client needs a unique integer specifying its execution order if multiple clients are connected
        // to a TraCI server concurrently. You don't need to modify this setting if only Artery is connected to SUMO.
        int clientId = default(1);
}
