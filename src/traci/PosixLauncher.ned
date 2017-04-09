package traci;

simple PosixLauncher like Launcher
{
    parameters:
        @class(traci::PosixLauncher);
        string sumo = default("sumo");
        string command = default("%SUMO% --remote-port %PORT% --configuration-file %SUMOCFG% --message-log sumo-%RUN%.log --no-step-log --quit-on-end");
        string sumocfg;
        int port = default(0);
        int seed = default(0);
}
