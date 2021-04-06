//
// Artery V2X Simulation Framework
// Copyright 2014-2018 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

// SeeThroughSensor has a field-of-view but is not affected by line-of-sight obstructions
simple SeeThroughSensor like FovSensor
{
    parameters:
        double fovRange @unit(m);
        double fovAngle = default(360); // opening angle in degree
        string attachmentPoint = default("FRONT");
        int numSegments = default(12);
        bool doLineOfSightCheck = false;
        bool drawLinesOfSight = false;

        bool drawSensorCone = default(false);
        bool drawBlockingObstacles = default(false);
        bool drawDetectedObjects = default(false);
}
