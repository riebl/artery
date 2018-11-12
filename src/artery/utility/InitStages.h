/*
* Artery V2X Simulation Framework
* Copyright 2018 Raphael Riebl
* Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
*/

#ifndef ARTERY_COMINITSTAGES_H_VWJCXRN0
#define ARTERY_COMINITSTAGES_H_VWJCXRN0

namespace artery
{

struct InitStages {
    enum {
        /**
         * Read local parameters and look up sibling modules
         */
        Prepare = 0,

        /**
         * Initialize module itself
         */
        Self = 1,

        /**
         * Propagate messages, events, and signals
         */
        Propagate = 3,

        /**
         * Total number of initialisation stages
         */
        Total
    };
};

} // namespace artery

#endif /* ARTERY_COMINITSTAGES_H_VWJCXRN0 */

