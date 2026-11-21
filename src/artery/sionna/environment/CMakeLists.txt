add_artery_feature(sionna-phy)

target_sources(sionna-phy
    PRIVATE
        PathLoss.cc
        Ground.cc
        PhysicalEnvironment.cc
        SionnaScalarAnalogModel.cc
        api/CoordinateTransform.cc
        api/DynamicSceneConfig.cc
        api/IDConverter.cc
        api/SionnaAPI.cc
        radio/SionnaAntennaArray.cc
        radio/SionnaPlanarAntennaArray.cc
        radio/SionnaRadioDeviceBase.cc
        radio/SionnaVanetTransmitter.cc
        radio/SionnaVanetReceiver.cc
        config/scenes/FileSceneConfigProvider.cc
        config/scenes/SionnaBuiltinSceneConfigProvider.cc
        config/meshes/SionnaAssetMeshRegistry.cc
        config/dynamic/DynamicSceneConfigListener.cc
        config/dynamic/SionnaActorBase.cc
        config/dynamic/SionnaPersonActor.cc
        config/dynamic/SionnaVehicleActor.cc
        config/dynamic/TraciDynamicSceneConfigProvider.cc
        visualization/SionnaSceneVisualizer.cc
    PUBLIC
        Compat.h
        PathLoss.h
        Ground.h
        PhysicalEnvironment.h
        SionnaScalarAnalogModel.h
        api/CoordinateTransform.h
        api/DynamicSceneConfig.h
        api/IDConverter.h
        api/SionnaAPI.h
        radio/SionnaAntennaArray.h
        radio/SionnaPlanarAntennaArray.h
        radio/SionnaRadioDeviceBase.h
        radio/SionnaVanetTransmitter.h
        radio/SionnaVanetReceiver.h
        config/meshes/IMeshRegistry.h
        config/scenes/IStaticSceneProvider.h
        config/scenes/FileSceneConfigProvider.h
        config/scenes/SionnaBuiltinSceneConfigProvider.h
        config/meshes/SionnaAssetMeshRegistry.h
        config/dynamic/DynamicSceneConfigListener.h
        config/dynamic/ISionnaActor.h
        config/dynamic/SionnaActorBase.h
        config/dynamic/SionnaPersonActor.h
        config/dynamic/SionnaVehicleActor.h
        config/dynamic/TraciDynamicSceneConfigProvider.h
        visualization/ISceneVisualizer.h
        visualization/SionnaSceneVisualizer.h
)

set_target_properties(sionna-phy
    PROPERTIES
        SIONNA_BRIDGE_COMPILATION_MODE Inet
        COMPATIBLE_INTERFACE_STRING "SIONNA_FLOAT_TYPE;SIONNA_SPECTRUM_TYPE"
)

target_link_libraries(sionna-phy PUBLIC sionna-bridge-inet-llvm-ad-mono-polarized)
# FIXME: should go with drjit fix.
target_compile_options(sionna-phy PUBLIC $<$<CONFIG:Debug>:-DNDEBUG>)
