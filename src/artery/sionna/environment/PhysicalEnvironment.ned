package artery.sionna.environment;

import inet.environment.contract.IPhysicalEnvironment;
import artery.sionna.environment.config.dynamic.TraciDynamicSceneConfigProvider;
import artery.sionna.environment.config.meshes.IMeshRegistry;
import artery.sionna.environment.config.scenes.IStaticSceneProvider;
import artery.sionna.environment.visualization.ISceneVisualizer;

//
// Extension of INET PhysicalEnvironment that mirrors the configured physical
// scene into a Sionna RT scene.
//
module SionnaPhysicalEnvironment like IPhysicalEnvironment
{
    parameters:
        @namespace(artery::sionna);
        @class(PhysicalEnvironment);

        // Scene providers. Keep source selection out of the environment itself.
        string sceneConfigProviderType = default("artery.sionna.environment.config.scenes.FileSceneConfigProvider");
        string meshRegistryType = default("artery.sionna.environment.config.meshes.SionnaDirectAssetMeshRegistry");
        string sceneVisualizerType = default("artery.sionna.environment.visualization.SionnaSceneVisualizer");
        string traciNodeManagerModule = default("^.traci.nodes");
        xml remapMatrix = default(xmldoc("transforms.xml", "/transforms/identity"));
        xml rotationMatrix = default(xmldoc("transforms.xml", "/transforms/identity"));
        xml translationVector = default(xmldoc("transforms.xml", "/transforms/zero"));

    submodules:
        sceneConfigProvider: <sceneConfigProviderType> like IStaticSceneProvider {
            @display("p=140,60");
        }

        dynamicSceneConfigProvider: TraciDynamicSceneConfigProvider {
            @display("p=140,100");
        }

        meshRegistry: <meshRegistryType> like IMeshRegistry if meshRegistryType != "" {
            @display("p=140,140");
        }

        sceneVisualizer: <sceneVisualizerType> like ISceneVisualizer if sceneVisualizerType != "" {
            @display("p=140,220");
        }
}
