package traci;

simple BasicModuleMapper like ModuleMapper
{
    parameters:
        @class(traci::BasicModuleMapper);
        string personType;
        string vehicleType;
        double penetrationRate = default(1.0);
        double personPenetrationRate = default(-1.0); // use penetrationRate when negative
        double vehiclePenetrationRate = default(-1.0); // use penetrationRate when negative
}
