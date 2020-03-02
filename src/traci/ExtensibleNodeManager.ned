package traci;

module ExtensibleNodeManager extends BasicNodeManager
{
    parameters:
        @class(traci::ExtensibleNodeManager);
        int numVehiclePolicies = default(0);

    submodules:
        vehiclePolicy[numVehiclePolicies]: <> like VehiclePolicy {
        }
}
