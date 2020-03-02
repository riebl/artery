package traci;

module InsertionDelayNodeManager extends ExtensibleNodeManager
{
    parameters:
        volatile double insertionDelay = default(uniform(0s, 1s)) @unit(s);

        numVehiclePolicies = 1;
        vehiclePolicy[0].typename = "InsertionDelayVehiclePolicy";
        vehiclePolicy[0].insertionDelay = insertionDelay;
}
