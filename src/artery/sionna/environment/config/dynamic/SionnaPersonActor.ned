package artery.sionna.environment.config.dynamic;

simple SionnaPersonActor extends artery.inet.PersonMobility
{
    parameters:
        @class(SionnaPersonActor);

        // Path to module implementing artery::sionna::IMeshRegistry.
        string meshRegistryModule = default("^.^.physicalEnvironment.meshRegistry");
}
