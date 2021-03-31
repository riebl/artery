package traci;

simple MultiTypeModuleMapper like ModuleMapper
{
    parameters:
        @class(traci::MultiTypeModuleMapper);
        xml personTypes = default(xml("<empty />"));
        xml vehicleTypes;
}
