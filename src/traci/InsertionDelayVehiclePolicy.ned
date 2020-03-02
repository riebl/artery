package traci;

//
// This policy defers insertion of vehicles by an configurable delay.
// By default, each vehicle has an individual random delay up to 1 second.
//
// This is useful to break artificial synchronisation between nodes when
// a) many vehicles are created at the same time
// b) vehicles are inserted at integer intervals larger than V2X periodicity
//
simple InsertionDelayVehiclePolicy like VehiclePolicy
{
    parameters:
        @class(traci::InsertionDelayVehiclePolicy);
        volatile double insertionDelay = default(uniform(0s, 1s)) @unit(s);
}
