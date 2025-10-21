package traci;

// PosixLauncher builds up sumo launch command and allows to
// format command arguments with various data. You may extend this
// formatting via this class. Take a look at EnvirArgumentFormatter for example.
moduleinterface IArgumentFormatter {}

simple EnvirArgumentFormatter like IArgumentFormatter {}

simple PosixLauncher like Launcher
{
    parameters:
        @class(traci::PosixLauncher);

        int numFormatters = default(1);

        // Most common SUMO CLI options.
        string command = default("sumo");
        string sumocfg;
        string messageLogFile = default("%RESULTDIR%/sumo-%RUN%.log");
        int port = default(0);
        int seed = default(23423);

        // Options for actually starting a SUMO instance.
        string label = default("artery");
        string traceFile = default("");
        bool traceGetters = default(false);

        // Additional SUMO command line options.
        string extraOptions = default("--no-step-log --quit-on-end");

    submodules:
        formatter[numFormatters]: <default("EnvirArgumentFormatter")> like IArgumentFormatter;
}
