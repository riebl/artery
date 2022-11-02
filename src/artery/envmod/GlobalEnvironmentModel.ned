//
// Artery V2X Simulation Framework
// Copyright 2014-2017 Hendrik-Joern Guenther, Raphael Riebl, Oliver Trauer
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod;

simple GlobalEnvironmentModel
{
    parameters:
        @signal[EnvironmentModel.refresh](type=GlobalEnvironmentModel);
        @display("i=misc/globe;is=s");

        string traciModule;
        string nodeMobilityModule;
        string identityRegistryModule;
        bool drawObstacles = default(false);
        bool drawVehicles = default(false);
        string obstacleTypes = default("");
}
