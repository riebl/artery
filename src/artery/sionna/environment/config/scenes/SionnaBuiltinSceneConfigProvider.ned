package artery.sionna.environment.config.scenes;

simple SionnaBuiltinSceneConfigProvider like IStaticSceneProvider
{
    parameters:
        @class(SionnaBuiltinSceneConfigProvider);

        string scene = default("munich");
}
