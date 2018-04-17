package traci;

module Manager
{
    parameters:
        @display("i=block/network2;is=s");
        nodes.coreModule = "^.core";
        nodes.mapperModule = "^.mapper";
        nodes.subscriptionsModule = "^.subscriptions";
        subscriptions.coreModule = "^.core";

    submodules:
        core : Core {
        }

        launcher : <default("ConnectLauncher")> like Launcher {
        }

        subscriptions: <default("BasicSubscriptionManager")> like SubscriptionManager {
        }

        nodes : <default("BasicNodeManager")> like NodeManager {
        }

        mapper: <default("BasicModuleMapper")> like ModuleMapper {
        }
}
