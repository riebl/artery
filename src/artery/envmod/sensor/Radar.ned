//
// Artery V2X Simulation Framework
// Copyright 2014-2018 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

moduleinterface Radar
{
    parameters:
        double fovRange @unit(m);
        double fovAngle; // opening angle in degree
        int numSegments;
        bool doLineOfSightCheck;
}
