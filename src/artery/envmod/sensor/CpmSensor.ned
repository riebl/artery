//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.envmod.sensor;

simple CpmSensor
{
    parameters:
        string identityRegistryModule;
        double validityPeriod @unit(s) = default(1.1s);
}
