//
// Artery V2X Simulation Framework
// Copyright 2014-2018 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

simple OmniscientSensor like FovSensor
{
    parameters:
        double fovRange @unit(m);
        double fovAngle = default(360); // opening angle in degree
        string attachmentPoint = default("FRONT");
        int numSegments = 12;
        bool doLineOfSightCheck = false;
}
