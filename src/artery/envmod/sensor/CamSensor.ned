//
// Artery V2X Simulation Framework
// Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

simple CamSensor
{
    parameters:
        string identityRegistryModule;
        double validityPeriod @unit(s) = default(1.1s);
}
