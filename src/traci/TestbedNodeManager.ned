package traci;

simple TestbedNodeManager extends BasicNodeManager like NodeManager
{
    parameters:
        @class(traci::TestbedNodeManager);
        string twinId;
        string twinName = default("twin");
}
