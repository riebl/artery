package traci;

module Manager
{
    parameters:
        @display("i=block/network2;is=s");
        nodes.coreModule = "^.core";
        nodes.mapperModule = "^.mapper";

    submodules:
        core : Core {
        }

        launcher : <default("ConnectLauncher")> like Launcher {
        }

        nodes : <default("BasicNodeManager")> like NodeManager {
        }

        mapper: <default("BasicModuleMapper")> like ModuleMapper {
        }
}
