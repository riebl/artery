//
// Artery V2X Simulation Framework
// Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

simple RadarSensor like FovSensor
{
    parameters:
        double fovRange @unit(m) = default(80.0 m);
        double fovAngle = default(60.0); // degree
        string attachmentPoint = default("FRONT");
        int numSegments = default(1);
        bool doLineOfSightCheck = default(true);

        bool drawSensorCone = default(false);
        bool drawDetectedObjects = default(false);
        bool drawBlockingObstacles = default(false);
        bool drawLinesOfSight = default(false);
}
