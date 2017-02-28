package traci;

simple BasicModuleMapper like ModuleMapper
{
    parameters:
        @class(traci::BasicModuleMapper);
        string vehicleType;
        double penetrationRate = default(1.0);
}
