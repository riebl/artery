package traci;

//
// Specialised node manager which will not create modules for new
// SUMO vehicles immediately but after an individual delay.
//
// This is useful to break artificial synchronisation between nodes when
// a) many vehicles are created at the same time
// b) vehicles are inserted at integer intervals larger than V2X periodicity
//
simple InsertionDelayNodeManager extends BasicNodeManager
{
    parameters:
        @class(traci::InsertionDelayNodeManager);
        volatile double insertionDelay = default(uniform(0s, 1s)) @unit(s);
}
