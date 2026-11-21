package artery.sionna.environment.config.dynamic;

simple SionnaVehicleActor extends artery.inet.VehicleMobility
{
    parameters:
        @class(SionnaVehicleActor);

        // Path to module implementing artery::sionna::IMeshRegistry.
        string meshRegistryModule = default("^.^.physicalEnvironment.meshRegistry");
}
