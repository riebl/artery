package traci;

//
// This policy enables only vehicles located within defined regions of interest.
// This is useful when running large scenarios.
//
simple RegionOfInterestVehiclePolicy like VehiclePolicy
{
    parameters:
        @class(traci::RegionOfInterestVehiclePolicy);
        xml regionsOfInterest = default(xml("<regions />"));
}
