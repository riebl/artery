//
// Artery V2X Simulation Framework
// Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
//

package artery.application;

import artery.application.cp.*;


// CpService ITS-G5 Service Module
module CpService like ItsG5Service
{
    parameters:
        // Number of Redundancy Mitigation Rules to use
        int numRedundancyMitigationRules = default(0);

    submodules:
        // Core implementation
        core: Core;
        // CPM Generation Rule
        generation: <default("StaticGeneration")> like GenerationRule;
        // Redundancy Mitigation Rules
        redundancyMitigation[numRedundancyMitigationRules]: <> like RedundancyMitigationRule;
}
