package traci;

//
// Specialised node manager which will only create modules for 
// SUMO vehicles if in a region of interest.
// This is useful when running large scenarios
//
simple RegionOfInterestNodeManager extends BasicNodeManager
{
    parameters:
        @class(traci::RegionOfInterestNodeManager);
        xml regionsOfInterest = default(xml("<regions />"));
}
