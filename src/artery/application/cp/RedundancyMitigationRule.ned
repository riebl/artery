//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application.cp;


// Interface of CpService Redundancy Mitigation Rules
moduleinterface RedundancyMitigationRule
{
}


// Distance-based Redundancy Mitigation Rule
simple DistanceRedundancy like RedundancyMitigationRule
{
    parameters:
        @class(cp::DistanceRedundancy);

        // Time window of received CPMs to examine
        int windowRedundancy @unit(ms) = default(1000ms);
        // Distance difference between transmitting ITS and the ITS the object was received from
        // that needs to get reached or exceeded for the object not to be omitted
        double distanceRedundancy @unit(m) = default(10m);
}


// Dynamics-based Redundancy Mitigation Rule
simple DynamicsRedundancy like RedundancyMitigationRule
{
    parameters:
        @class(cp::DynamicsRedundancy);

        // Distance difference between last transmitted and current position
        // that needs to get exceeded for the object not to be omitted
        double distanceRedundancy @unit(m) = default(4m);
        // Speed difference between last transmitted and current speed
        // that needs to get exceeded for the object not to be omitted
        double speedRedundancy @unit(mps) = default(0.5mps);
}


// Frequency-based Redundancy Mitigation Rule
simple FrequencyRedundancy like RedundancyMitigationRule
{
    parameters:
        @class(cp::FrequencyRedundancy);

        // Time window of received CPMs to examine
        int windowRedundancy @unit(ms) = default(1000ms);
        // Number of times an object is contained that needs to get exceeded to omit the object
        int numberRedundancy = default(5);
}


// Object Self-Announcement Redundancy Mitigation Rule
simple SelfAnnouncementRedundancy like RedundancyMitigationRule
{
    parameters:
        @class(cp::SelfAnnouncementRedundancy);
}
