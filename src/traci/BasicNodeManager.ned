package traci;

simple BasicNodeManager like NodeManager
{
    parameters:
        @class(traci::BasicNodeManager);
        @signal[traci.node.add](type=string);
        @signal[traci.node.update](type=string);
        @signal[traci.node.remove](type=string);
        string coreModule;
        string mapperModule;
        string vehicleSinkModule;
}
