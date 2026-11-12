package artery.sionna.environment.config.scenes;

simple FileSceneConfigProvider like IStaticSceneProvider
{
    parameters:
        @class(FileSceneConfigProvider);

        string scenePath;
        bool mergeMeshes = default(true);
        bool mergeEquivalent = default(true);
}
