package artery.sionna.environment.config.meshes;

//
// Provider for runtime mesh lookup by logical asset id.
//
moduleinterface IMeshRegistry
{
}

simple SionnaDirectAssetMeshRegistry like IMeshRegistry
{
    parameters:
        @class(SionnaDirectAssetMeshRegistry);
}
