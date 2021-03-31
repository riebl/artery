package traci;

simple BasicNodeManager like NodeManager
{
    parameters:
        @class(traci::BasicNodeManager);
        @signal[traci.node.add](type=string);
        @signal[traci.node.update](type=unsigned long);
        @signal[traci.node.remove](type=string);
        @signal[traci.person.add](type=string);
        @signal[traci.person.update](type=string);
        @signal[traci.person.remove](type=string);
        @signal[traci.vehicle.add](type=string);
        @signal[traci.vehicle.update](type=string);
        @signal[traci.vehicle.remove](type=string);
        string coreModule;
        string mapperModule;
        string personSinkModule;
        string vehicleSinkModule;
        string subscriptionsModule;
        bool destroyVehiclesOnCrash = default(false);
        bool ignorePersons;
}
