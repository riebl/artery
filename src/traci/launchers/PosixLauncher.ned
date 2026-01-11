package traci;

simple PosixLauncher like Launcher
{
    parameters:
        @class(traci::PosixLauncher);

        // Default options for starting up SUMO - you likely will not need others
        // in simple scenarios.
        string sumo = default("sumo");
        string sumocfg;
        // You may remove RESULTDIR and RUN template arguments here safely.
        string messageLog = default("%RESULTDIR%/sumo-%RUN%.log");
        int port = default(0);
        int seed = default(23423);

        // Additional SUMO command line options.
        string extraOptions = default("--no-step-log --quit-on-end");
}
