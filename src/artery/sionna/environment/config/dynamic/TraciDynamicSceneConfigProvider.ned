package artery.sionna.environment.config.dynamic;

simple TraciDynamicSceneConfigProvider
{
    parameters:
        @class(TraciDynamicSceneConfigProvider);

        // Path to traci::BasicNodeManager (or compatible emitter of TraCI add/remove signals).
        string traciNodeManagerModule = default("^.^.traci.nodes");
}
