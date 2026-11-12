package artery.sionna.environment.visualization;

simple SionnaSceneVisualizer like ISceneVisualizer
{
    parameters:
        @namespace(artery::sionna);
        @class(SionnaSceneVisualizer);

        string outputDir = default("renders");
        string camera = default("");
        string pathLoss = default(absPath("^.^.radioMedium.pathLoss"));
        xml cameraConfig = default(null);
        int spp = default(16);
        int width = default(1280);
        int height = default(720);
}
