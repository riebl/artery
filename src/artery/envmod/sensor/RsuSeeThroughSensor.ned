//
// Artery V2X Simulation Framework
// Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

simple RsuSeeThroughSensor extends SeeThroughSensor
{
    parameters:
        @class(RsuSeeThroughSensor);
        attachmentPoint = "FRONT"; // irrelevant for RSU sensors
        double fovHeading = default(90.0); // degree (OMNeT++ coordinate system!)
}
