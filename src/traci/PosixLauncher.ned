package traci;

simple PosixLauncher like Launcher
{
    parameters:
        @class(traci::PosixLauncher);
        string sumo = default("sumo");
        string command = default("%SUMO% --remote-port %PORT% --seed %SEED% --configuration-file %SUMOCFG% --message-log sumo-%RUN%.log --no-step-log --quit-on-end");
        string sumocfg;
        int port = default(0);
        int seed = default(23423);

        // additional SUMO command line options
        string extraOptions = default("");
}
